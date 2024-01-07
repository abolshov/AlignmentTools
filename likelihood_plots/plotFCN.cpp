#include <iostream>
#include <vector>

#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMarker.h"
#include "TGraph2D.h"
#include "TMultiGraph.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TLegend.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "No file name provided, shutting down.\n";
        return 0;
    }

    char const* plots = argv[1];

    std::vector<std::string> par_names{"dx", "dy", "dz", "dphix", "dphiy", "dphiz"};

    std::vector<double> min{0,0,0,-0.001,0.002,-0.003};
    // std::vector<double> calc{0.0008814953675, 0.0003422438033, 0.0007250105923, -0.0009827540176, 0.001959195753, -0.003001869471};
    // std::vector<double> calc{2.190701597e-05, 0.000189086595, 0.0003711052282, -0.000987294831, 0.001957391141, -0.002997829718 };
    std::vector<double> calc{0.000881503,0.000342246,0.000725021,-0.000982754,0.0019592,-0.00300187}; 
    double fcn_min = -3075605.148; // likelihood value at true min
    double fcn_calc = -3075848.501; // likelihood value at computed min

    // double fcn_min = -79644174.37; // likelihood value at true min
    // double fcn_calc = -79650549.55; // likelihood value at computed min


    if (argc >= 2 && plots)
    {
        TFile* plot_file = new TFile(plots, "READ");
        TFile* cont_file = nullptr;
        if (argc > 2) 
        {
            char const* contours = argv[2];
            cont_file = new TFile(contours, "READ");
        }
        TStyle* gStyle = new TStyle();

        char buffer[150];
        for (int par1 = 0; par1 < 6; ++par1)
        {
            for (int par2 = par1 + 1; par2 < 6; ++par2)
            {
                TCanvas* c1 = new TCanvas("c1", "c1");
                c1->SetLeftMargin(0.15);
                c1->SetRightMargin(0.15);
                c1->SetGrid();
                c1->SetTickx();
                c1->SetTicky();

                std::string plot_name("graph2d_");
                plot_name += par_names[par1] + "_vs_" + par_names[par2];
                TGraph2D* graph = static_cast<TGraph2D*>(plot_file->Get(plot_name.c_str()));
                gStyle->SetPalette(1);
                char const* name_x = par_names[par1].c_str();
                char const* name_y = par_names[par2].c_str();
                sprintf(buffer, "FCN(%s, %s, const); %s; %s", name_x, name_y, name_x, name_y);
                graph->SetTitle(buffer);
                graph->Draw("colz");

                TLegend* leg = new TLegend(0.15, 0.1, 0.35, 0.3);

                if (cont_file)
                {
                    char cont_name[30];
                    sprintf(cont_name, "1sig_cnt_%s_%s", name_x, name_y);
                    TGraph* cont = static_cast<TGraph*>(cont_file->Get(cont_name));
                    if (cont)
                    {
                        cont->SetLineWidth(2);
                        cont->Draw("same");
                        leg->AddEntry(cont, "1 sigma", "l");
                    }
                    sprintf(cont_name, "2sig_cnt_%s_%s", name_x, name_y);
                    cont = static_cast<TGraph*>(cont_file->Get(cont_name));
                    if (cont)
                    {
                        cont->SetLineWidth(2);
                        cont->Draw("same");
                        leg->AddEntry(cont, "2 sigma", "l");
                    }
                    sprintf(cont_name, "3sig_cnt_%s_%s", name_x, name_y);
                    cont = static_cast<TGraph*>(cont_file->Get(cont_name));
                    if (cont)
                    {
                        cont->SetLineWidth(2);
                        cont->Draw("same");
                        leg->AddEntry(cont, "3 sigma", "l");
                    }
                }

                TMarker* m_true;
                if (!min.empty())
                {
                    m_true = new TMarker(min[par1], min[par2], 23);
                    m_true->SetMarkerColor(6);
                    m_true->SetMarkerSize(2);
                    m_true->Draw();
                    leg->AddEntry(m_true, "true min", "p");
                }

                TMarker* m_calc;
                if (!calc.empty())
                {
                    m_calc = new TMarker(calc[par1], calc[par2], 22);
                    m_calc->SetMarkerColor(1);
                    m_calc->SetMarkerSize(2);
                    m_calc->Draw();
                    leg->AddEntry(m_calc, "calc min", "p");
                }

                if (!calc.empty() || !min.empty()) leg->Draw();

                // c1->SaveAs(("fcn_graphs/" + plot_name + ".png").c_str());
                c1->SaveAs(Form("fcn_graphs/%s.png", plot_name.c_str()));

                delete c1;
                delete graph;
                delete leg;
                if (!min.empty()) delete m_true;
                if (!calc.empty()) delete m_calc;
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
            TGraph* graph = static_cast<TGraph*>(plot_file->Get(name.c_str()));
            graph->SetLineWidth(3);
            graph->SetLineColor(4);
            graph->GetXaxis()->SetTitle(par_names[par].c_str());
            graph->Draw("AL");
            sprintf(buffer, "FCN(%s, const)", par_names[par].c_str());
            graph->SetTitle(buffer);

            TLegend* leg = new TLegend(0.1, 0.1, 0.3, 0.3);

            TMarker* m_true;
            if (!min.empty() && fcn_min != 0.0)
            {
                m_true = new TMarker(min[par], fcn_min, 23);
                m_true->SetMarkerColor(2);
                m_true->SetMarkerSize(2);
                m_true->Draw();
                leg->AddEntry(m_true, "true min", "p");
            }

            TMarker* m_calc;
            if (!calc.empty())
            {
                m_calc = new TMarker(calc[par], fcn_calc, 22);
                m_calc->SetMarkerColor(3);
                m_calc->SetMarkerSize(2);
                m_calc->Draw();
                leg->AddEntry(m_calc, "calc min", "p");
            }

            if (!calc.empty() && !min.empty()) leg->Draw();

            c1->SaveAs(("fcn_graphs/graph1d_" + par_names[par] + ".png").c_str());

            delete c1;
            delete graph;
            delete leg;
            if (!min.empty()) delete m_true;
            if (!calc.empty()) delete m_calc;
        }

        if (cont_file)
        {
            char buffer[150];
            for (int par1 = 0; par1 < 6; ++ par1)
            {
                for (int par2 = par1 + 1; par2 < 6; ++par2)
                {
                    TCanvas* c1 = new TCanvas("c1", "c1");
                    c1->SetLeftMargin(0.15);
                    c1->SetRightMargin(0.15);
                    c1->SetGrid();
                    c1->SetTickx();
                    c1->SetTicky();

                    char const* name_x = par_names[par1].c_str();
                    char const* name_y = par_names[par2].c_str();

                    sprintf(buffer, "Contours %s vs %s; %s; %s", name_x, name_y, name_x, name_y);

                    TLegend* leg = new TLegend(0.15, 0.1, 0.35, 0.3);
                    char cont_name[30];
                    sprintf(cont_name, "2sig_cnt_%s_%s", name_x, name_y);
                    TGraph* cont = static_cast<TGraph*>(cont_file->Get(cont_name));
                    if (cont)
                    {
                        cont->SetLineWidth(2);
                        cont->Draw();
                        leg->AddEntry(cont, "2 sigma", "l");
                    }
                    sprintf(cont_name, "1sig_cnt_%s_%s", name_x, name_y);
                    cont = static_cast<TGraph*>(cont_file->Get(cont_name));
                    if (cont)
                    {
                        cont->SetLineWidth(2);
                        cont->Draw("same");
                        leg->AddEntry(cont, "1 sigma", "l");
                    }
                    sprintf(cont_name, "3sig_cnt_%s_%s", name_x, name_y);
                    cont = static_cast<TGraph*>(cont_file->Get(cont_name));
                    if (cont)
                    {
                        cont->SetLineWidth(2);
                        cont->Draw("same");
                        leg->AddEntry(cont, "3 sigma", "l");
                    }

                    TMarker* m_true;
                    if (!min.empty())
                    {
                        m_true = new TMarker(min[par1], min[par2], 23);
                        m_true->SetMarkerColor(6);
                        m_true->SetMarkerSize(2);
                        m_true->Draw();
                        leg->AddEntry(m_true, "true min", "p");
                    }

                    TMarker* m_calc;
                    if (!calc.empty())
                    {
                        m_calc = new TMarker(calc[par1], calc[par2], 22);
                        m_calc->SetMarkerColor(1);
                        m_calc->SetMarkerSize(2);
                        m_calc->Draw();
                        leg->AddEntry(m_calc, "calc min", "p");
                    }

                    if (!calc.empty() || !min.empty()) leg->Draw();

                    c1->SaveAs(Form("fcn_graphs/contours_%s_vs_%s.png", name_x, name_y));

                    delete c1;
                    delete leg;
                    if (!min.empty()) delete m_true;
                    if (!calc.empty()) delete m_calc;
                }
            }
        }

        plot_file->Close();
        if (cont_file) cont_file->Close();
        delete plot_file;
        delete cont_file;
    }

    return 0;
}
