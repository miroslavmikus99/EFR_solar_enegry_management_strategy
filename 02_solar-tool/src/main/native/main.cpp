#include <iostream>
#include <omp.h>

#include "model/solarSim.h"
#include "model/efrctrlr.h"

#include <arg/core/cAmphorA.h>
#include <arg/utils/cCLParser.h>

#include "cForest.h"
#include "cData.h"
#include "cGenProg.h"

#include "model/efr/cEFRModel.h"

arg::cAmphorA amphora;

void usage_mine(arg::cCLParser & cl)
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time(&rawtime);

#ifdef __GNUC__
    timeinfo = localtime(&rawtime);
#else
    struct tm ti;
    localtime_s(&ti, &rawtime);
    timeinfo = &ti;
#endif
    strftime(buffer, 80, "%Y", timeinfo);

    cout << "\nProgram " << cl.Program() << " for the fuzzy rule evolution.";
    cout << "\n\nBuild " << __DATE__ << " " << __TIME__;
#ifdef SCM_VERSION
    cout << ", SCM version [" << QUOTE(SCM_VERSION) << "]";
#endif
    cout << ".\nAmphorA version " << amphora.Identify() << ".\n\n";
    cout << "\nPavel Krömer <pavel.kromer@vsb.cz>, (c) 2011 - " << buffer << "\n\n";

    cout << "Usage:" << endl;
    cout << "\t" << cl.Program() << " -file <filename> [options]" << endl;
    cout << "\nOptions:\n";

    cout << "\t-file\t\tstring\t file with simulation data.\n";
    cout << "\t-fit\t\tinteger\t fitness. 0 - fscore; 1 - w arithm mean. Default is 0.\n";
    cout << "\t-maxinst\tinteger\t max. no of instructions in the tree. Default is 200.\n";
    cout << "\t-vv\tbool\t display fitness details.\n\n";

    cout << "\t-gen\t\tint\t number of generations to process (1000)\n";
    cout << "\t-pop\t\tint\t population size (100)\n";
    cout << "\t-c\t\tdouble\t crossover rate (0.8)\n";
    cout << "\t-m\t\tdouble\t mutation rate (0.02)\n";

    cout << "\t-sel\t\tint\t selection type (2)\n";
    cout << "\t\t\t\t selection types: 0 - roulette, 1 - elitary, 2 - semielitary\n";
    cout << "\t-mig\t\tint\t migration type (301)\n";
    cout << "\t\t\t\t migration types: 301 - steady state, 302 - steady state with reverse fitness\n";
    cout << "\t-shuffle\t\t shuffle candidates to prevent stagnation (false)\n";
    cout << "\t-prevent\t\t prevent duplicate candidates to prevent stagnation (false)\n";
    cout << "\t-term-feedback\tint\t for time series; defines the past level of terms (0)\n";
    cout << "\t-out-feedback\tint\t for time series; defines the past level of output node (0)\n";

    cout << "\t-b\t\tbool\t ban NOT operator (false)\n";
    cout << "\t-dot\t\tbool\t print winner in DOT language (false)\n";
    cout << "\t-threads\tint\t number of threads to use (1)\n";
    cout << "\n\n";
    cout << "\t-query\t\tstring\t a query. Evaluate a query instead of evolution.\n";
    //cout << "\n\n";
    //cout << "\t--tune\t\tbool\t perform fine tuning by DE after rule evolution.\n";
    cout << "\n\n";
}

arg::cIndividual * gen_alg(arg::cCLParser & cl, cData& data, cModel& model, cSolarMdlSim * sim)
{
    int pop_size = cl.Integer("pop", 100);
    int sel = cl.Integer("sel", arg::cGA::SELECT_SEMIELITARY);
    int mig = cl.Integer("mig", arg::cGA::STEADY_STATE);
    int limit = cl.Integer("gen", 1000);

    double pC = cl.Double("c", 0.8);
    double pM = cl.Double("m", 0.02);

    const bool debug = cl.Boolean("d");

    cForest::t_FitnessType fit_type = (cForest::t_FitnessType) cl.Integer("fit", cForest::t_FitnessType::FIT_FSCORE);

    cout << "Initializing GA." << endl;
    cGenProg ga(fit_type, pop_size, data, model, debug);

    ga.SelectionType(sel);
    ga.MigrationType(mig);

    cout << "Genetic algorithm initialized." << endl << endl;

    bool prevent = cl.Boolean("prevent");

    int i;
    arg::cTimer timer;
    timer.CpuStart();
    unsigned int evals = pop_size;

    cout << "\t\t\tgen\teval\ttime[ms]\tfitness\t\tP1\t\tP2";
    if (cl.Boolean("vv"))
    {
        cout << "\t\t|\tFailD \tFailT \tFailM \tTrnsOk \tMeasOk \tOvchCnt\tE_Unused \tBuffLst\tBuffSizeAvg";
    }
    cout << endl;
    cout << "--------------------------------------------------------------------------------------------------";
    
    if (cl.Boolean("vv"))
    {
        cout << "----------------------------------------------------------------------------------------------";
    }
    
    cout << endl;

    double winner_fit = 0;

    cForest * winner = NULL;

    cout << std::fixed << std::setprecision(6);

    cSimStats run_stats;

    double best_fit = 0;

    for (i = 0; i < limit; i++)
    {
        ga.Select();
        ga.Recombine(pC);
        ga.Mutate(pM);
        ga.ComputeChildFitness();
        ga.Migrate(prevent);
        evals += 2;

        if (cl.Boolean("shuffle"))
            ga.Shuffle();

        winner = (cForest*) ga.WinnerPtr();
        winner_fit = winner->Fitness();

        if (winner_fit > best_fit)
        {
            best_fit = winner_fit;

            cout << "[" << &ga << "]\t" << i << "\t" << evals << "\t" << timer.CpuStop().CpuMillis()
                 << "\t";
                
            cout << winner_fit << "\t" << winner->P1() << "\t" << winner->P2();

            // this will be VERY SLOW
            if (cl.Boolean("vv"))
            {
                winner->Evaluate();
                sim->calcStats(&run_stats);
                cout << "\t|\t";
                cout << run_stats.FailD << "\t";
                cout << run_stats.FailT << "\t";
                cout << run_stats.FailM << "\t";
                cout << run_stats.TransOk << "\t";
                cout << run_stats.MeasOk << "\t";
                cout << run_stats.OvchCnt << "\t";
                cout << run_stats.E_Unused << "\t";
                cout << run_stats.BuffLost << "\t";
                cout << run_stats.BuffSizeAvg;
            }
            cout << endl;
        }
        else if (i % 100 == 0)
        {
            cout << "[" << &ga << "]\t" << i << "\t" << evals << "\t" << timer.CpuStop().CpuMillis()
                    << "\t";
            // ga.PrintPopulationInfo();
            cout << winner_fit << "\t" << winner->P1() << "\t" << winner->P2();

            // this will be VERY SLOW
            if (cl.Boolean("vv"))
            {
                winner->Evaluate();
                sim->calcStats(&run_stats);
                cout << "\t|\t";
                cout << run_stats.FailD << "\t";
                cout << run_stats.FailT << "\t";
                cout << run_stats.FailM << "\t";
                cout << run_stats.TransOk << "\t";
                cout << run_stats.MeasOk << "\t";
                cout << run_stats.OvchCnt << "\t";
                cout << run_stats.E_Unused << "\t";
                cout << run_stats.BuffLost << "\t";
                cout << run_stats.BuffSizeAvg;
            }
            cout << endl;
        }
    }

    winner = (cForest*) ga.WinnerPtr();
    cout << std::fixed << "[" << &ga << "]\t" << i << "\t" << evals << "\t" << timer.CpuStop().CpuMillis() << "\t";
    cout << winner->Fitness() << "\t" << winner->P1() << "\t" << winner->P2();
    if (cl.Boolean("vv"))
    {
        winner->Evaluate();
        sim->calcStats(&run_stats);
        cout << "\t|\t";
        cout << run_stats.FailD << "\t";
        cout << run_stats.FailT << "\t";
        cout << run_stats.FailM << "\t";
        cout << run_stats.TransOk << "\t";
        cout << run_stats.MeasOk << "\t";
        cout << run_stats.OvchCnt << "\t";
        cout << run_stats.E_Unused << "\t";
        cout << run_stats.BuffLost << "\t";
        cout << run_stats.BuffSizeAvg;
    }
    cout << endl;

    cout << "Fitness\t" << ga.WinnerPtr()->Fitness() << endl;
    cout << endl;
    return ga.WinnerPtr()->Clone();
}

void mine(arg::cCLParser & cl)
{
    cSolarMdlSim * sim = new cSolarMdlSim();

    char * query = (char *) cl.String("query");
    double beta = cl.Double("beta", 1.0);

    unsigned int threads = cl.Integer("threads", 1);
    omp_set_num_threads(threads);

    cForest::t_FitnessType fit_type = (cForest::t_FitnessType) cl.Integer("fit", cForest::t_FitnessType::FIT_FSCORE);

    if (cl.Boolean("h"))
    {
        usage_mine(cl);
    }
    else
    {
        cout << cl << "#" << endl;
        cout << "#\tNo of threads " << threads << endl;

        cEFRModel model;
        model.Debug(cl.Boolean("d"));
        model.Nontrivial(cl.Boolean("nt"));
        model.MaxTreeInstructions(cl.Integer("maxinst", 200));
        model.Beta(beta);

        const char * file = cl.String("file", "<none>");

        if (sim->loadDataFile(file))
        {
            cout << "#\tLoaded simulation file \'" << file << "\'\n";
            cout << "#\tRows:" << sim->getDataLength()  << endl;
            cData data(sim->getDataLength(), 4);
            model.SolarModel(sim); // cEFRModel deletes the object

            cout << "#\tPrepared data buffer with " << data.Records() << "x" << data.Inputs() << " records.\n";
            int out_feedback = cl.Integer("of", 0);
            int term_feedback = cl.Integer("if", 0);

            if (cl.Boolean("b"))
            {
                cout << "\t#Banning NOT operator" << endl;
                model.NotIsAllowed(false);
            }
            model.PastInputLimit(cl.Integer("if", 0));

            model.PastOutputLimit(out_feedback);
            model.PastInputLimit(term_feedback);


            if (query == NULL)
            {
                cForest * winner = (cForest*) gen_alg(cl, data, model, sim);

                double win_fit = winner->Fitness();

                cout << "Fitness " << win_fit << endl;

                cout << endl;
                winner->Print();
                cout << "-------------- " << endl;
                winner->ComputeFitness();

                if (cl.Boolean("v"))
                {
                    cout << data.m_Data << endl;
                }

                cout << std::scientific << "Fitness   : " << win_fit << endl;
                cSimStats* stats = sim->calcStats();
                stats->print();

                if (cl.Boolean("dot"))
                {
                    cout << "Dot " << endl;
                    winner->Dot();
                    cout << "-------------- " << endl;
                }

                delete winner;
                delete stats;
            }
            else // do the querying
            {
                cForest forest(data, model, fit_type);
                forest.ParseForest(query);
                forest.ComputeFitness();


                cSimStats* stats = sim->calcStats();

                if (!cl.Boolean("compact"))
                {
					cout << "Fitness   : " << forest.Fitness() << endl;
					stats->print();
					cout << endl;

					forest.Print();
					cout << "-------------- " << endl;

					if (cl.Boolean("dot"))
					{
						cout << "Dot " << endl;
						forest.Dot();
						cout << "-------------- " << endl;
					}
                }
                else
                {
                	// compact print
                	cout << fixed << forest.Fitness() << "\t";
                	cout << "\t|\t";
                	cout << stats->FailD << "\t";
                	cout << stats->FailT << "\t";
                	cout << stats->FailM << "\t";
                	cout << stats->TransOk << "\t";
                	cout << stats->MeasOk << "\t";
                	cout << stats->OvchCnt << "\t";
                	cout << stats->E_Unused << "\t";
                	cout << stats->BuffLost << "\t";
                	cout << stats->BuffSizeAvg;
                	cout << endl;

                }

                delete stats;
            }
        }
        else
        {
            cerr << "Could not load input file \'" << file << "\'.\n";
        }
    }
}

void test(arg::cCLParser & cl)
{
    cSolarMdlSim * sim = new cSolarMdlSim();

    char * query = (char *) cl.String("query");
    double beta = cl.Double("beta", 1.0);

    cForest::t_FitnessType fit_type = (cForest::t_FitnessType) cl.Integer("fit", cForest::t_FitnessType::FIT_FSCORE);

    {
        cout << cl << endl;

        cEFRModel model;
        model.Debug(cl.Boolean("d"));
        model.Nontrivial(cl.Boolean("nt"));
        model.MaxTreeInstructions(cl.Integer("maxinst", 200));
        model.Beta(beta);

        const char * file = cl.String("file", "DataSim_O1MOSN01_2016.csv");

        if (sim->loadDataFile(file))
        {
            cout << "Loaded simulation file \'" << file << "\'\n";
            cout << "Rows:" << sim->getDataLength()  << endl;
            cData data(sim->getDataLength(), 4);
            model.SolarModel(sim); // cEFRModel deletes the object

            cout << "Prepared data buffer with " << data.Records() << "x" << data.Inputs() << " records.\n";
            int out_feedback = cl.Integer("of", 0);
            int term_feedback = cl.Integer("if", 0);

            if (cl.Boolean("b"))
            {
                cout << "Banning NOT operator" << endl;
                model.NotIsAllowed(false);
            }
            model.PastInputLimit(cl.Integer("if", 0));

            model.PastOutputLimit(out_feedback);
            model.PastInputLimit(term_feedback);

            if (query == NULL)
            {
                //generate random controler
                cForest * winner = new cForest(data, model, fit_type);
                winner->ComputeFitness();

                double win_fit = winner->Fitness();

                cout << "Fitness " << win_fit << endl;

                cout << endl;
                winner->Print();
                cout << "-------------- " << endl;
                winner->ComputeFitness();

                if (cl.Boolean("v"))
                {
                    cout << data.m_Data << endl;
                }

                cout << std::scientific << "Fitness   : " << win_fit << endl;
                cSimStats* stats = sim->calcStats();
                stats->print();


                if (cl.Boolean("dot"))
                {
                    cout << "Dot " << endl;
                    winner->Dot();
                    cout << "-------------- " << endl;
                }

                delete winner;
                delete stats;
            }
            else // do the querying
            {
                cForest forest(data, model, fit_type);
                forest.ParseForest(query);
                forest.ComputeFitness();
                cSimStats* stats = sim->calcStats();

                if (!cl.Boolean("compact"))
                {
					cout << "Fitness   : " << forest.Fitness() << endl;
					stats->print();
					cout << endl;

					forest.Print();
					cout << "-------------- " << endl;

					if (cl.Boolean("dot"))
					{
						cout << "Dot " << endl;
						forest.Dot();
						cout << "-------------- " << endl;
					}
                }
                else
                {
                	// compact print
                	cout << fixed << forest.Fitness() << "\t";
                	cout << "\t|\t";
                	cout << stats->FailD << "\t";
                	cout << stats->FailT << "\t";
                	cout << stats->FailM << "\t";
                	cout << stats->TransOk << "\t";
                	cout << stats->MeasOk << "\t";
                	cout << stats->OvchCnt << "\t";
                	cout << stats->E_Unused << "\t";
                	cout << stats->BuffLost << "\t";
                	cout << stats->BuffSizeAvg;
                	cout << endl;

                }

                delete stats;
            }
        }
        else
        {
            cerr << "Could not load input file \'" << file << "\'.\n";
        }
    }
}

int main(int argc, const char* argv[])
{
    arg::cCLParser cl(argc, argv);
    const unsigned int seed = cl.Integer("seed", 0);

    if (seed > 0)
    {
        arg::cStaticRandom::Seed(seed);
    }

    if (cl.Boolean("-test"))
    {
        test(cl);
    }
    else
    {
        mine(cl);
    }

    return 0;
}

