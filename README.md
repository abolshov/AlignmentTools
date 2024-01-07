# AlignmentTools

A repository for tools used to debug and visualize results of "Big Structure" alignment (or "Global Mode" alignment) of CMS muon system with [GPR fitter](https://github.com/abolshov/cmssw/blob/from-CMSSW_12_4_4/Alignment/MuonAlignmentAlgorithms/interface/MuonResidualsGPRFitter.h)

# Currently contains:
- Script for making 2D level plots, contour plots and 1D slice plots of the likelihood function minimized by the fitter
- Script for fitting and visualizing plots of residual distributions: distributions are fitted to a gaussian, then $`\chi^2/N_{\text{dof}}`$ is computed


# How to use:
- To build and run plotting script do
  ```
  cd likelihood_plots/
  ./run.sh file_with_plots.root file_with_contours.root
  ```
  ```file_with_contours.root``` is optional; if specified, contour plots will be drawn on top of level plots, otherwise only level plots will be made. If no file is provided program will finish with an error message. Results will be saved in ```fcn_graphs/``` folder.
- To build and run comparison script do
  ```
  cd residual_plots/
  make cmp
  ./cmp 
  ```
  Input files must be specified in the program. $`\chi^2/N_{\text{dof}}`$ is saved in ```.txt``` files for each residual distribution. Plots are saved in ```resid_distr/``` folder.
