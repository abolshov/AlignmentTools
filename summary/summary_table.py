import numpy as np 
import csv
import argparse


def ReadFromFile(file_name):
    with open(file_name, "r") as file:
        res_lines = file.readlines()
        res = np.array([list(map(float, x.rstrip().split(" "))) for x in res_lines])
        return res


def main():
    parser = argparse.ArgumentParser(prog='summary_table', description='Saves summary tables with alignment results')
    parser.add_argument('error_fname', type=str, help="File containing alignment errors")
    parser.add_argument('values_fname', type=str, help="File containing computed values")
    parser.add_argument('tests_fname', type=str, help="File containing true shifts")
    parser.add_argument('subsystem', type=str, help="subsytem: DT or CSC; for csc should specify endcap 1 or 2 (E1/E2)")

    args = parser.parse_args()
    
    err = ReadFromFile(args.error_fname)
    val = ReadFromFile(args.values_fname)
    true_vals = ReadFromFile(args.tests_fname)

    sigmas = np.abs(true_vals + val)/err
    np.set_printoptions(formatter={'float': '{: 0.3f}'.format})
    print(sigmas)

    param_names = np.array(['dx', 'dy', 'dz', 'dphix', 'dphiy', 'dphiz'])

    rows, cols = sigmas.shape
    results_file = args.subsystem + "_res.csv"
    with open(results_file, 'w') as csvfile:
        writer = csv.writer(csvfile, delimiter=',')
        writer.writerow(param_names)
        for row in range(rows):
            words = [f"{v:.5} " + u"\u00B1" + f" {e:.5}\n\t{t:.5}" for v, e, t in zip(val[row], err[row], true_vals[row])]
            writer.writerow(words)

    nsig_file = args.subsystem + "_nsig.csv"
    with open(nsig_file, "w") as csvfile:
        writer = csv.writer(csvfile, delimiter=',')
        writer.writerow(param_names)
        for row in range(rows):
            writer.writerow([f"{s:.3}" for s in sigmas[row]])


if __name__ == "__main__":
    main()
