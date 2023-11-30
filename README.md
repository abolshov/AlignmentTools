# AlignmentTools

A repository for tools used to debug and visualize results of "Big Structure" alignment (or "Global Mode" alignment) of CMS muon system with [GPR fitter](https://github.com/abolshov/cmssw/blob/from-CMSSW_12_4_4/Alignment/MuonAlignmentAlgorithms/interface/MuonResidualsGPRFitter.h)

# Currently contains:
- Script for making 2D level plots, contour plots and 1D slice plots of the likelihood function minimized by the fitter

# How to use:
- To build and run plotting script do
  ```
  ./run.sh file_with_plots.root file_with_contours.root
  ```
  ```file_with_contours.root``` is optional; if specified, contour plots will be drawn on top of level plots, otherwise only level plots will be made. If no file is provided program will finish with an error message.
