#include <iostream>
#include <vector>
#include <memory>
#include <fstream>

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
#include "TTree.h"
#include "THStack.h"
#include "TString.h"
#include "TF1.h"

constexpr int nbins = 100;
constexpr double resid_range = 10.0;
constexpr double slope_range = 0.1;

struct ResidDistr
{
    std::unique_ptr<TH1F> residX_hist;
    std::unique_ptr<TH1F> residY_hist;
    std::unique_ptr<TH1F> residSlopeX_hist;
    std::unique_ptr<TH1F> residSlopeY_hist;
};

// store goodness of fit (GoF) = chi2/ndof
struct GoF
{
    double residX_gof;
    double residY_gof;
    double residSlopeX_gof;
    double residSlopeY_gof;
};

void save_1d_stack(std::vector<TH1F*> const& distrs,
                   std::vector<std::string> const& legends,
                   std::string const& name,
                   std::string const& title,
                   std::string const& axis_label)
{
    if (distrs.size() != legends.size())
    {
        std::cout << "number of legends and histograms do not match!" << std::endl;
        return;
    }
    TCanvas* c1 = new TCanvas("c1", "c1");
    c1->SetGrid();
    c1->SetTickx();
    c1->SetTicky();

    THStack* stack = new THStack("stack", title.c_str());
    auto legend = new TLegend(0.7, 0.7, 0.9, 0.9);

    for (int i = 0; i < static_cast<int>(distrs.size()); ++i)
    {
        distrs[i]->SetLineWidth(3);
        int line_color = i + 1 < 5 ? i + 1 : i + 2;
        distrs[i]->SetLineColor(line_color);
        stack->Add(distrs[i]);
        legend->AddEntry(distrs[i], legends[i].c_str(), "l");
    }

    stack->Draw("nostack");
    stack->GetXaxis()->SetTitle(axis_label.c_str());
    legend->Draw();
    c1->SaveAs((name + ".png").c_str());

    delete stack;
    delete legend;
    delete c1;
}

ResidDistr GetHistograms(TTree* t, std::vector<char const*> const& hist_names)
{
    Double_t residX, residY, residSlopeX, residSlopeY;

    auto residX_hist = std::make_unique<TH1F>(hist_names[0], hist_names[0], nbins, -resid_range, resid_range);
    auto residY_hist = std::make_unique<TH1F>(hist_names[1], hist_names[1], nbins, -resid_range, resid_range);
    auto residSlopeX_hist = std::make_unique<TH1F>(hist_names[2], hist_names[2], nbins, -slope_range, slope_range);
    auto residSlopeY_hist = std::make_unique<TH1F>(hist_names[3], hist_names[3], nbins, -slope_range, slope_range);

    t->SetBranchAddress("residX", &residX);
    t->SetBranchAddress("residY", &residY);
    t->SetBranchAddress("residSlopeX", &residSlopeX);
    t->SetBranchAddress("residSlopeY", &residSlopeY);

    for (int i = 0; i < t->GetEntries(); ++i)
    {
        t->GetEntry(i);
        residX_hist->Fill(residX);
        residY_hist->Fill(residY);
        residSlopeX_hist->Fill(residSlopeX);
        residSlopeY_hist->Fill(residSlopeY);
    }

    return {std::move(residX_hist), std::move(residY_hist), std::move(residSlopeX_hist), std::move(residSlopeY_hist)};
}

GoF FitHistograms(ResidDistr& distr)
{
    auto& [residX, residY, slopeX, slopeY] = distr;

    auto residX_func = std::make_unique<TF1>("residX_func", "gaus", -resid_range, resid_range);
    residX_func->SetParameter(1, 1);
    residX_func->SetParameter(2, 1);
    residX_func->SetParameter(0, residX->GetEntries());

    auto residY_func = std::make_unique<TF1>("residY_func", "gaus", -resid_range, resid_range);
    residY_func->SetParameter(1, 1);
    residY_func->SetParameter(2, 1);
    residY_func->SetParameter(0, residY->GetEntries());

    auto residSlopeX_func = std::make_unique<TF1>("residSlopeX_func", "gaus", -slope_range, slope_range);
    residSlopeX_func->SetParameter(1, 0);
    residSlopeX_func->SetParameter(2, 1);
    residSlopeX_func->SetParameter(0, slopeX->GetEntries());

    std::cout << slopeX->GetEntries() << "\n";

    auto residSlopeY_func = std::make_unique<TF1>("residSlopeY_func", "gaus", -slope_range, slope_range);
    residSlopeY_func->SetParameter(1, 0);
    residSlopeY_func->SetParameter(2, 1);
    residSlopeY_func->SetParameter(0, slopeY->GetEntries());

    residX->Fit(residX_func.get());
    residY->Fit(residY_func.get());
    slopeX->Fit(residSlopeX_func.get());
    slopeY->Fit(residSlopeY_func.get());

    return {residX_func->GetChisquare()/residX_func->GetNDF(),
            residY_func->GetChisquare()/residY_func->GetNDF(),
            residSlopeX_func->GetChisquare()/residSlopeX_func->GetNDF(),
            residSlopeY_func->GetChisquare()/residSlopeY_func->GetNDF()};
}

int main()
{
    auto all_file = std::make_unique<TFile>("resid_fid_cut.root", "READ"); // file containning UNCUT resid distr
    auto cut_file = std::make_unique<TFile>("residuals.root", "READ"); // file containing CUT resid distr: residuals.root contains fid cut + peakNsigma cut, which is WRONG
    // auto cut_file = std::make_unique<TFile>("resid_all.root", "READ");

    std::ofstream all_gof("all.txt");
    std::ofstream cut_gof("cut.txt");

    std::vector<int> wheels{-2, -1, 0, 1, 2};
    std::vector<int> stations{1, 2, 3};
    std::vector<int> chambers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    char tree_name[20];

    std::vector<char const*> all_names{"residX_hist_a", "residY_hist_a", "residSlopeX_hist_a", "residSlopeY_hist_a"};
    std::vector<char const*> cut_names{"residX_hist_c", "residY_hist_c", "residSlopeX_hist_c", "residSlopeY_hist_c"};

    double residX_avg_gof_all = 0;
    double residY_avg_gof_all = 0;
    double slopeX_avg_gof_all = 0;
    double slopeY_avg_gof_all = 0;

    double residX_avg_gof_cut = 0;
    double residY_avg_gof_cut = 0;
    double slopeX_avg_gof_cut = 0;
    double slopeY_avg_gof_cut = 0;
    int n_cham = 0;

    for (auto const& wheel: wheels)
    {
        for (auto const& station: stations)
        {
            for (auto const& chamber: chambers)
            {
                std::sprintf(tree_name, "%d:%d:%d_tree", wheel, station, chamber);
                TTree* all_tree = static_cast<TTree*>(all_file->Get(tree_name));
                TTree* cut_tree = static_cast<TTree*>(cut_file->Get(tree_name));
            
                ResidDistr all_dist = GetHistograms(all_tree, all_names);
                ResidDistr cut_dist = GetHistograms(cut_tree, cut_names);

                // saving plots comparing cut and uncut residual distributions
                auto& [residX_hist_1, residY_hist_1, residSlopeX_hist_1, residSlopeY_hist_1] = all_dist;
                auto& [residX_hist_2, residY_hist_2, residSlopeX_hist_2, residSlopeY_hist_2] = cut_dist;

                std::vector<std::string> leg{ "only fiducial cut", "fiducial + tail cut" };
                save_1d_stack({residX_hist_1.get(), residX_hist_2.get()}, 
                              leg, 
                              Form("resid_distr/residX_cmp_%s", tree_name), 
                              Form("residX: chamber %d/%d/%d", wheel, station, chamber), 
                              "resid");
                save_1d_stack({residY_hist_1.get(), residY_hist_2.get()}, 
                              leg, 
                              Form("resid_distr/residY_cmp_%s", tree_name), 
                              Form("residY: chamber %d/%d/%d", wheel, station, chamber),
                              "resid");
                save_1d_stack({residSlopeX_hist_1.get(), residSlopeX_hist_2.get()}, 
                              leg, 
                              Form("resid_distr/residSlopeX_cmp_%s", tree_name), 
                              Form("residSlopeX: chamber %d/%d/%d", wheel, station, chamber), 
                              "resid");
                save_1d_stack({residSlopeY_hist_1.get(), residSlopeY_hist_2.get()}, 
                              leg, 
                              Form("resid_distr/residSlopeY_cmp_%s", tree_name), 
                              Form("residSlopeY: chamber %d/%d/%d", wheel, station, chamber), 
                              "resid");

                // fit cut and uncut residual distribution and compare chi2/ndof
                auto [residX_gof, residY_gof, slopeX_gof, slopeY_gof] = FitHistograms(all_dist);
                auto [residX_gof_cut, residY_gof_cut, slopeX_gof_cut, slopeY_gof_cut] = FitHistograms(cut_dist);

                residX_avg_gof_all += residX_gof;
                residY_avg_gof_all += residY_gof;
                slopeX_avg_gof_all += slopeX_gof;
                slopeY_avg_gof_all += slopeY_gof;

                residX_avg_gof_cut += residX_gof_cut;
                residY_avg_gof_cut += residY_gof_cut;
                slopeX_avg_gof_cut += slopeX_gof_cut;
                slopeY_avg_gof_cut += slopeY_gof_cut;

                all_gof << wheel << "/" << station << "/" << chamber << ": " << residX_gof << " " << residY_gof << " " << slopeX_gof << " " << slopeY_gof << "\n";
                cut_gof << wheel << "/" << station << "/" << chamber << ": " << residX_gof_cut << " " << residY_gof_cut << " " << slopeX_gof_cut << " " << slopeY_gof_cut << "\n";

                ++n_cham;
            }
        }
    }

    std::cout << "Average goodness of fits:\n";
    residX_avg_gof_all /= n_cham;
    residY_avg_gof_all /= n_cham;
    slopeX_avg_gof_all /= n_cham;
    slopeY_avg_gof_all /= n_cham;

    residX_avg_gof_cut /= n_cham;
    residY_avg_gof_cut /= n_cham;
    slopeX_avg_gof_cut /= n_cham;
    slopeY_avg_gof_cut /= n_cham;

    std::cout << "\tall: " << residX_avg_gof_all << " " << residY_avg_gof_all << " " << slopeX_avg_gof_all << " " << slopeY_avg_gof_all << "\n";
    std::cout << "\tcut: " << residX_avg_gof_cut << " " << residY_avg_gof_cut << " " << slopeX_avg_gof_cut << " " << slopeY_avg_gof_cut << "\n";

    return 0;
}