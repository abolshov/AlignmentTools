#include <iostream>
#include <memory>

#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"

int main()
{
    auto fPtr = std::make_unique<TFile>("residuals.root", "READ");

    std::vector<int> wheels{-2, -1, 0, 1, 2};
    std::vector<int> stations{1, 2, 3};
    std::vector<int> chambers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    char tree_name[20];
    for (auto wheel: wheels)
    {
        int wheel_avg = 0;
        for (auto station: stations)
        {
            int station_avg = 0;
            for (auto chamber: chambers)
            {
                std::sprintf(tree_name, "%d:%d:%d_tree", wheel, station, chamber);
                TTree* t = static_cast<TTree*>(fPtr->Get(tree_name));
                wheel_avg += t->GetEntries();
                station_avg += t->GetEntries();
                // std::cout << "chamber " << wheel << "/" << station << "/" << chamber << ": " << t->GetEntries() << "\n";
            }
            std::cout << "\tstation " << station << " average: " << static_cast<int>(station_avg*1.0/12) << "\n";
            // std::cout << "----------------------\n";
        }
        std::cout << "wheel " << wheel << " average: " << static_cast<int>(wheel_avg*1.0/36) << "\n";
        // std::cout << "========================\n";
    }

    return 0;

    TTree* treePtr = static_cast<TTree*>(fPtr->Get("2:2:12_tree"));

    auto residX_hist = std::make_unique<TH1F>("residX_hist", "residX_hist", 100, -3, 1.5);
    auto residY_hist = std::make_unique<TH1F>("residY_hist", "residY_hist", 100, -2.0, 2.0);
    auto residSlopeX_hist = std::make_unique<TH1F>("residSlopeX_hist", "residSlopeX_hist", 100, -0.02, 0.02);
    auto residSlopeY_hist = std::make_unique<TH1F>("residSlopeX_hist", "residSlopeX_hist", 100, -0.02, 0.02);

    Double_t residX, residY, residSlopeX, residSlopeY;

    treePtr->SetBranchAddress("residX", &residX);
    treePtr->SetBranchAddress("residY", &residY);
    treePtr->SetBranchAddress("residSlopeX", &residSlopeX);
    treePtr->SetBranchAddress("residSlopeY", &residSlopeY);

    Int_t nEntries = treePtr->GetEntries();
    for (Int_t i = 0; i < nEntries; ++i)
    {
        treePtr->GetEntry(i);
        residX_hist->Fill(residX);
        residY_hist->Fill(residY);
        residSlopeX_hist->Fill(residSlopeX);
        residSlopeY_hist->Fill(residSlopeY);
    }

    // auto residX_func = std::make_unique<TF1>("residX_func", "gaus", -3.0, 1.5);
    // residX_func->SetParameter(1, 1);
    // residX_func->SetParameter(2, -0.5);
    // residX_func->SetParameter(0, residX_hist->GetEntries());
    // residX_hist->Fit(residX_func.get(), "R");

    // int ndf = residX_func->GetNDF();
    // double chi2 = residX_func->GetChisquare();
    // std::cout << "chi2/ndf = " << chi2 << "/" << ndf << " = " << chi2/ndf << "\n";

    // auto c1 = std::make_unique<TCanvas>("c1", "c1");
    // residX_hist->Draw();
    // c1->SaveAs("residX_hist.png");

    auto residSlopeX_func = std::make_unique<TF1>("residSlopeX_hist", "gaus", -0.02, 0.02);
    residSlopeX_func->SetParameter(1, 1);
    residSlopeX_func->SetParameter(2, 0);
    residSlopeX_func->SetParameter(0, residSlopeX_hist->GetEntries());
    residSlopeX_hist->Fit(residSlopeX_func.get(), "R");

    int ndf = residSlopeX_func->GetNDF();
    double chi2 = residSlopeX_func->GetChisquare();
    std::cout << "chi2/ndf = " << chi2 << "/" << ndf << " = " << chi2/ndf << "\n";

    auto c1 = std::make_unique<TCanvas>("c1", "c1");
    residSlopeX_hist->Draw();
    c1->SaveAs("residSlopeX_hist.png");

    return 0;
}