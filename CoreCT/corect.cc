#include <ctime>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#include <csignal>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <assert.h>

#include <net/if.h>


#ifdef DARWIN
#include <net/if_dl.h>
#include <ifaddrs.h>
#include <net/if_types.h>
#else //!DARWIN
// #include <linux/if.h>
// #include <linux/sockios.h>
#endif //!DARWIN


#include "DetectorConstruction.hh"
//#include "RoundDetectorConstruction.hh"
#include "PhysicsList.hh"
#include "HistManager.hh"
#include "MessagesHandler.hh"
#include "TargetsManager.hh"
#include "TargetsCalculator.hh"
#include "RunAction.hh"
#include "EventAction.hh"

#include "DBManager.hh"

#include "corect_ggo.h"

#include <G4UserRunAction.hh>
#include <G4RunManager.hh>
#include <G4MTRunManager.hh>
#include <G4UImanager.hh>
#include <G4VisExecutive.hh>
#include <G4UIQt.hh>
#include <G4VUserActionInitialization.hh>
#include <Randomize.hh>
#include <G4UIExecutive.hh>

#include "FTFP_BERT.hh"

#include "UnitTransport.hh"
#include "DispatcherTransport.hh"
#include "Geant4CalcUnitChannel.hh"
#include "Geant4CalcChannelsAggregator.hh"
#include "Geant4CalcDispatcherChannel.hh"

#include "Exception.hh"


// Process gengetopt with config file option

extern args_info_corect args_info;
args_info_corect args_info;
//args_info_##ggo_filename args_info;
#define GGO(ggo_filename, args_info) \
    cmdline_parser_##ggo_filename##_params args_params; \
    cmdline_parser_##ggo_filename##_params_init(&args_params); \
    args_params.print_errors = 1; \
    args_params.check_required = 1; \
    args_params.override = 1; \
    args_params.initialize = 1; \
    \
    if(0 != cmdline_parser_##ggo_filename##_ext(argc, argv, &args_info, &args_params) ) \
{ \
    std::cerr << "Error in cmdline_parser_" #ggo_filename "_ext" << std::endl; \
    exit(1); \
    }
class MyActionInitialization: public G4VUserActionInitialization
{
public:
    MyActionInitialization() {}

    void Build() const
    {
        SetUserAction(TargetsManager::instance()->createPrimaryGeneratorAction());
        SetUserAction(new G4UserRunAction);
        SetUserAction(new EventAction);
    }
    void BuildForMaster() const
    {
        SetUserAction(new G4UserRunAction);
    }

private:

};


//---------------------------------get MAC addresses ------------------------------------unsigned short-unsigned short----------
// we just need this for purposes of unique machine id. So any one or two mac's is fine.
unsigned hashMacAddress( unsigned char* mac )
{
    unsigned hash = 0;

    for ( int i = 0; i < 6; i++ )
    {
        hash += ( mac[i] << (( i & 1 ) * 8 ));
    }
    return hash;
}

unsigned getMacHash( )
{
    unsigned mac = 0;

#ifdef DARWIN

    struct ifaddrs* ifaphead;
    if ( getifaddrs( &ifaphead ) != 0 )
        return 0;

    // iterate over the net interfaces
    struct ifaddrs* ifap;
    for ( ifap = ifaphead; ifap; ifap = ifap->ifa_next )
    {
        struct sockaddr_dl* sdl = (struct sockaddr_dl*)ifap->ifa_addr;
        if ( sdl && ( sdl->sdl_family == AF_LINK ) && ( sdl->sdl_type == IFT_ETHER ))
        {
            mac = hashMacAddress( (unsigned char*)(LLADDR(sdl))); //sdl->sdl_data) + sdl->sdl_nlen) );
            if (mac) break;
        }
    }

    freeifaddrs( ifaphead );

#else // !DARWIN

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP );
    if ( sock < 0 ) return 0;

    // enumerate all IP addresses of the system
    struct ifconf conf;
    char ifconfbuf[ 128 * sizeof(struct ifreq)  ];
    memset( ifconfbuf, 0, sizeof( ifconfbuf ));
    conf.ifc_buf = ifconfbuf;
    conf.ifc_len = sizeof( ifconfbuf );
    if ( ioctl( sock, SIOCGIFCONF, &conf ))
    {
        return 0;
    }

    // get MAC address
    struct ifreq* ifr;
    for ( ifr = conf.ifc_req; (char*)ifr < (char*)conf.ifc_req + conf.ifc_len; ifr++ )
    {
        if ( ifr->ifr_addr.sa_data == (ifr+1)->ifr_addr.sa_data )
            continue;  // duplicate, skip it

        if ( ioctl( sock, SIOCGIFFLAGS, ifr ))
            continue;  // failed to get flags, skip it
        if ( ioctl( sock, SIOCGIFHWADDR, ifr ) == 0 )
        {
            mac = hashMacAddress( (unsigned char*)(ifr->ifr_addr.sa_data));
            if (mac) break;
        }
    }

    close( sock );

#endif // !DARWIN

    return mac;

}

int numberOfProcessors()
{
    unsigned macHash = getMacHash();
    int num = 1;

    if (macHash == 70514) // i7 (fna)
        num = 5;
    else if (macHash == 86583) // my i5-7600
        num = 3;
    else if (macHash == 62174) // 9700K (bunker29)
        num = 5;
    else if (macHash == 24902) // bunker_main
        num = 8;//sysconf(_SC_NPROCESSORS_ONLN);//
    else num = sysconf(_SC_NPROCESSORS_ONLN);

    return num;
}
#include <math.h>

#define PI 3.14159265

G4RunManager* runManager = nullptr;
G4VisManager* visManager = nullptr;
G4UIExecutive* UIExecutive = nullptr;

void init(int argc, char**argv)
{
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
    CLHEP::HepRandom::setTheSeed(time(NULL));

    TargetsManager::instance()->setTarget(args_info.target_arg);

    if (args_info.ncores_arg != 1)
    {
        G4MTRunManager* mtRunManager = new G4MTRunManager;
        mtRunManager->SetNumberOfThreads(args_info.ncores_arg ? args_info.ncores_arg : numberOfProcessors());
        runManager = mtRunManager;
    }
    else
        runManager = new G4RunManager;

    runManager->SetUserInitialization(new PhysicsList);

    //DetectorConstruction* detector_c = new DetectorConstruction();
    runManager->SetUserInitialization(TargetsManager::instance()->createDetectorConstruction());

    runManager->SetUserInitialization(new MyActionInitialization());

    if (!args_info.distributed_flag)
    {
        //if (!args_info.mac_given)
        //{
        visManager = new G4VisExecutive;
        visManager->Initialize();
        //}

        UIExecutive = new G4UIExecutive(argc,argv);
    }


    MessagesHandler::instance()->init(runManager, UIExecutive);
    //	TargetsManager::instance()->initHistogram(); // it's better to init TargetsManager at the beginning of the DetectorConstruction::Construct()
    // in order to name dump files that depend on the configration params

    G4UImanager * UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand("/control/execute init.mac");

    if (args_info.mac_given)
    {
        // batch mode
        //command line contains name of the macro to execute
        G4String command = "/control/execute ";
        G4String fileName = args_info.mac_arg;
        UImanager->ApplyCommand(command+fileName);
    }
    else if (!args_info.distributed_flag)
    {
        if (UIExecutive->IsGUI())
            UImanager->ApplyCommand("/control/execute visQt.mac");
        else //if... TODO: what if ogl is not available ? there should be console mac
            UImanager->ApplyCommand("/control/execute visGL.mac");
    }


    printf("INIT ---------------\n");

}

void free()
{
    HistManager::free();
    TargetsManager::free();
    MessagesHandler::free();
    TargetsCalculator::free();


    if (UIExecutive) delete UIExecutive;
    if (visManager) delete visManager;
    delete runManager;

    printf("DEINIT ---------------\n");
}

class App: public IApp
{
public:
    App(int argc, char**argv) : argc_(argc), argv_(argv) {}

    void threadInitialization() override
    {
        init(argc_, argv_);
    }

    void threadDeinitialization() override
    {
        free();
    }

    void run(int events) override
    {
        runManager->BeamOn(events);
    }

    void fillBufferWithGatheredResults(char* buffer, int *len) override
    {
        HistManager::instance()->fillBufferWithGatheredResults("my", buffer, len);
    }

private:
    int argc_;
    char** argv_;
};


bool restart(Geant4CalcChannelsAggregator *agg, const char *szParams, bool update)
{
    printf("che za h %d\n", int(update));

    if (!update)
        agg->restart(szParams);
    else
    {
        int r = system("tar cvf 1.tar.bz2 --exclude='build' --exclude='out' .."); (void)r;
        agg->update(szParams);
    }

    int unitsToWait = agg->unitsNumber();
    agg->disconnect();

    if (!unitsToWait)
    {
        printf("no connections.. exit..\n");
        return false;
    }

    int num;

    DispatcherTransport* trans = agg->trans();

    do
    {
        num = trans->unitConnections().size();
        printf("wait for update unit connections... %d (%d)\n", num, unitsToWait);
        if (num >= unitsToWait) break;
        sleep(1);
    }
    while (1);

    agg->setChannels(trans->unitConnections());

    return true;
}


void pipe_signal_handler(int signal)
{

}

int main(int argc, char** argv)
{

    printf("unic machine id (based on mac): %u\n", getMacHash());

    GGO(corect, args_info);

    srand(time(NULL));

    std::signal(SIGPIPE, pipe_signal_handler);

    printf("params number %d: \n", argc);
    for (int i = 0; i < argc; i++)
        printf("%s ", argv[i]);
    printf("\n");
    remove("params");

    TargetsCalculator::instance()->SetValuesByTarget(args_info.target_arg);

    //Distributed

    if (!args_info.distributed_flag) init(argc, argv);

    if (args_info.distributed_flag)
    {
        if (args_info.dispatcher_flag)
        {
            /*
             * This scope must be written by user of distributed library
             * BEGIN
             * (TODO: should bring to corresponding class)
             */

            // создаёт транспорт диспетчера и ищёт апнутые юниты
            DispatcherTransport trans;
            trans.sendBroadCastMessage("192.168.0.0");
            std::list<int> lst = trans.unitConnections();

            // создаёт аггрегатор для удобства работы с юнитами. можно и без него через класс Geant4CalcDispatcherChannel, и работать с каждым юнитом отдельно, но это в исключительных ситуациях. когда каждый юнит нужно обработывать по своему.
            Geant4CalcChannelsAggregator agg(&trans);
            agg.setChannels(trans.unitConnections());

            //agg.update("d --ncores=4 --angle=0"

            //restart(&agg, "-d --ncores=0 --angle=0", true);
            //agg.stop();

            static bool firstTime = true;

            for (G4int Index = 19; Index < 360; Index++)  ///////////////////////////////////////////////////цикл здесь
            {

                float Angle = Index*1.0;
                G4String Anglee = std::to_string(Angle);
                G4String Command = "-d -h --ncores=0 --angle=" + Anglee;

                const char *cstr = Command.c_str();

                // эта функция должна уйти в distributed модуль, пока болтаётся портянкой в main.cpp.
                // прототип bool restart(Geant4CalcChannelsAggregator *agg, const char *szParams, bool update)
                // agg - агрегатор, szParams - парамы на вход проги на юните, update - нужно ли перекомпиливать код на юните
                restart(&agg, cstr, firstTime);

                firstTime = false;

                // стартует расчёт. функция не блочная, за фриз здесь не переживать =) далее будет поллинг со слипом
                // в параме скока ивентов всего нужно насчитать, 0 - безлим
                agg.run(0);

                //short Temptime = 0;

                while(1)
                {
                    sleep(1);

                    /*for (std::list<int>::iterator itr = lst.begin(); itr != lst.end(); itr++)
                    {
                        char *p = agg.getInfo(*itr);
                        if (p)
                        {
                            unsigned temp = agg.temperature(*itr);
                            if (temp >= 73)
                            {
                                Temptime ++;
                                std::cout << "!!!!UNIT is hot boy, check it: unit - " << p << "has temperature - " << temp << std::endl;
                                std::cout << "It's " << Temptime << temp << std::endl;
                                if (Temptime > 5)
                                {
                                    std::cout << "its more than 6 time high temperature wrong situation, i'm out" << std::endl;
                                    exit(1);
                                }
                            }
                        }
                    }*/

                    unsigned long long e = agg.eventProcessed();
                    if (e > 200000000LL)
                    break;

                }

                // тормозим юниты
                agg.stop();


                // скачиваем результаты
                int len;
                agg.downloadStats(trans.buffer(), &len);

                // сохраняем результаты
                // пока хардкод - инит гистограммы должен быть 1 в 1 как ините гистограммы в таргете. потом переделаю это место
                std::vector<int> dim;
                dim.push_back(TargetsCalculator::instance()->GetDetYNumber());
                char sz[512];
                sprintf(sz, "%.1f/%%d.txt", Angle);
                //HistManager::instance()->initHistogram("my", sz, dim, 0, 560, 560);
                HistManager::instance()->initHistogram("my", sz, dim, 0, TargetsCalculator::instance()->GetDetXNumber(), TargetsCalculator::instance()->GetDetXNumber());

                /*std::vector<int> dim;
            dim.push_back(84/4);
            char sz[512];
            sprintf(sz, "%.1f/%%d.txt", args_info.angle_arg);
            //HistManager::instance()->initHistogram("my", sz, dim, 0, 560, 560);
            HistManager::instance()->initHistogram("my", sz, dim, 0, 448/4, 448/4);*/

                HistManager::instance()->saveHisto("my", trans.buffer(), false, "matlab-compability", false);
                HistManager::instance()->free();
            }



            //
            // это всё. работа с телеметрией и всяка така хрень по желанию ;)

            /*
             * This scope must be written by user of distributed library
             * END
             */


#if 0
            DBManager db;
            db.open();
            db.cleanDB();

            /*std::vector<int> dim;
            dim.push_back(84);
            char sz[512];*/

            /*std::vector<int> dim;
            dim.push_back(84/4);
            dim.push_back(448/4);
            char sz[512];*/

            DispatcherTransport trans;
            trans.sendBroadCastMessage("192.168.0.255");
            std::list<int> lst = trans.unitConnections();

            if (!lst.size())
            {
                printf("no units.. exit.. \n");
                return 0;
            }

            Geant4CalcChannelsAggregator agg(&trans);
            agg.setChannels(trans.unitConnections());

            bool first = true;
            std::map<std::string, unsigned long long> mapEvents;
            std::map<std::string, unsigned long long> mapEvents1;

            int angle;
            int angleTo = 2;
            for (angle = 0; angle < angleTo; angle+=2)
            {
                db.setProgress((int)((angle / float(angleTo))*100));

                char params[200];
                //sprintf(params, "-d --ncores=0 --angle=%d -t KernSpectralProcessing", angle);
                sprintf(params, "-d --ncores=0 --angle=%d --comp-density=%.2f --filter-thick=%.2f ", angle, args_info.comp_density_arg, args_info.filter_thick_arg);
                if (args_info.i0shot_flag)
                    strcat(params, "--i0shot ");
                if (!restart(&agg, params, args_info.with_update_flag && first)) return 1;
                first = false;
                agg.run(0);
                int c = 0;
                while (1)
                {
                    FILE *fstats = fopen("stats", "w");
                    FILE *ftemps = fopen("temps", "w");
                    lst = trans.unitConnections();
                    db.beginTransaction();
                    for (std::list<int>::iterator itr = lst.begin(); itr != lst.end(); itr++)
                    {
                        char *p = agg.getInfo(*itr);
                        if (p)
                        {
                            if (!mapEvents.count(p))
                                mapEvents[p] = 0;

                            mapEvents1[p] = agg.eventProcessed(*itr);
                            unsigned long long e = mapEvents1[p] + mapEvents[p];
                            unsigned temp = agg.temperature(*itr);
                            printf("Unit %s processed %llu, temp %d°C\n", p, e, temp);
                            fprintf(fstats, "%s\n%llu\n", p, e);
                            fprintf(ftemps, "%s\n%u\n", p, temp);
                            db.addTempValue(p, temp);
                            db.updateEventProcessedValue(p, e);
                        }
                    }
                    db.endTransaction();
                    fclose(fstats);
                    fclose(ftemps);

                    unsigned long long e = agg.eventProcessed();

                    printf("common calculated: %llu\n", e);

                    if (e >= 10000000000LL)
                        //if (e >= 10000000LL)
                        break;

                    /*if (!(c++ % 10))
                    {
                        agg.stop();
                        int len;
                        agg.downloadStats(trans.buffer(), &len);
                        HistManager::instance()->saveHisto("my", trans.buffer());
                        agg.run(0);
                    }*/

                    sleep(10);

                }

                for (std::map<std::string, unsigned long long>::iterator itr = mapEvents1.begin(); itr != mapEvents1.end(); itr++)
                    mapEvents[itr->first] += itr->second;

                agg.stop();

                int len;
                agg.downloadStats(trans.buffer(), &len);
                HistManager
                        std::vector<int> dim;
                dim.push_back(84/4);
                char sz[512];
                sprintf(sz, "%.1f/%s/%%d.txt", args_info.disc_density_arg, args_info.i0shot_flag ? "gain" : "0.0");
                HistManager::instance()->initHistogram("my", sz, dim, 0, 448/4, 448/4);

                HistManager::instance()->saveHisto("my", trans.buffer(), false, "matlab-compability", true);
                HistManager::instance()->free();

                db.cleanDB();
            }
            db.setProgress((int)((angle / 360.)*100));

            agg.disconnect();
#endif
        }
        else
        {
            App app(argc, argv);

            UnitTransport trans;

            bool alreadyConnected = false;
            if (!access("dispatcher_host", F_OK))
            {
                alreadyConnected = trans.connectToDispatcher("dispatcher_host");
                remove("dispatcher_host");
            }

            Geant4CalcUnitChannel calcCh(&app, &trans);

            char *buffer;
            int len;
            bool keepRunning = true;
            do
            {
                if (!alreadyConnected) trans.waitForDispatcherConnection();

                try
                {
                    while (keepRunning && trans.listenARead(&buffer, &len))
                        if (!calcCh.process(buffer, len))
                            keepRunning = false;
                }
                catch (const Exception& exception)
                {
                    printf("An error has occured for fd %d: %s\n", exception.fd(), exception.errorStr().c_str());
                }

                trans.disconnect();
                alreadyConnected = false;
            }
            while (keepRunning);
        }
    }


    TargetsCalculator::instance()->SaveDetectorParams("ParamHeader.txt");
    TargetsCalculator::instance()->free();


    if (!args_info.distributed_flag)
    {TargetsManager::instance()->saveHisto();
        free();}


    return 0;
}


