#include <iostream>
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TF1.h"
#include <math.h>
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TRandom3.h"
#include "TGraph2D.h"
#include "TMultiGraph.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TDirectory.h"
#include "TMarker.h"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <functional>
#include <numeric>
#include "TRandom3.h"
#include <cmath>
#include <random>
#include <cassert>
#include <algorithm>

int main(int argc, char* argv[])
{
    char const* filename = argv[1];

    if (argc < 2 && filename == nullptr)
    {
        std::cout << "No file name provided, shutting down.\n";
        return 0;
    }

    std::vector<std::string> par_names{"dx", "dy", "dz", "dphix", "dphiy", "dphiz"};

    if (argc == 2 && filename)
    {
        std::cout << "Executing with " << filename << "\n";

        TFile* file_1 = new TFile(filename, "READ");
        TStyle* gStyle = new TStyle();

        std::vector<double> min{-0.13942,0.47498,0.22497,-0.000392,0.000413,7.80781e-05};

        char buffer[100];
        for (int par1 = 0; par1 < 6; ++par1)
        {
            for (int par2 = par1 + 1; par2 < 6; ++par2)
            {
                TCanvas* c1 = new TCanvas("c1", "c1");
                c1->SetGrid();
                c1->SetTickx();
                c1->SetTicky();
                std::string name("graph2d_");
                name += par_names[par1] + "_vs_" + par_names[par2];
                TGraph2D* graph = static_cast<TGraph2D*>(file_1->Get(name.c_str()));
                gStyle->SetPalette(1);
                graph->Draw("colz");
                sprintf(buffer, "FCN(%s, %s, const)", par_names[par1].c_str(), par_names[par2].c_str());
                graph->SetTitle(buffer);

                auto m = new TMarker(min[par1], min[par2], 104); 
                // m->SetMarkerStyle(104);
                m->SetMarkerColor(1);
                m->SetMarkerSize(3);
                m->Draw();
                c1->SaveAs(("fcn_graphs/" + name + ".png").c_str());
                delete c1;
                delete graph;
            }
        }

        for (int par = 0; par < 6; ++par)
        {
            TCanvas* c1 = new TCanvas("c1", "c1");
            c1->SetGrid();
            c1->SetTickx();
            c1->SetTicky();
            std::string name("graph1d_");
            name += par_names[par];
            TGraph* graph = static_cast<TGraph*>(file_1->Get(name.c_str()));
            graph->SetLineWidth(3);
            graph->SetLineColor(4);
            graph->Draw("AL");
            sprintf(buffer, "FCN(%s, const)", par_names[par].c_str());
            graph->SetTitle(buffer);
            c1->SaveAs(("fcn_graphs/graph1d_" + par_names[par] + ".png").c_str());
            delete c1;
            delete graph;
        }
    }
    return 0;
}
