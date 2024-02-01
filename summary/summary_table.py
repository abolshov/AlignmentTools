import numpy as np 
import csv


def ReadFromFile(file_name):
    with open(file_name, "r") as file:
        res_lines = file.readlines()
        res = np.array([list(map(float, x.rstrip().split(" "))) for x in res_lines])
        return res


def main():
    err = ReadFromFile("errors.txt")
    val = ReadFromFile("values.txt")
    true_vals = ReadFromFile("tests.txt")

    sigmas = np.abs(true_vals + val)/err
    np.set_printoptions(formatter={'float': '{: 0.3f}'.format})
    print(sigmas)

    param_names = np.array(['dx', 'dy', 'dz', 'dphix', 'dphiy', 'dphiz'])

    rows, cols = sigmas.shape
    with open("res.csv", 'w') as csvfile:
        writer = csv.writer(csvfile, delimiter=',')
        writer.writerow(param_names)
        for row in range(rows):
            words = [f"{v:.5} " + u"\u00B1" + f" {e:.5}\n\t{t:.5}" for v, e, t in zip(val[row], err[row], true_vals[row])]
            writer.writerow(words)

    with open("nsig.csv", "w") as csvfile:
        writer = csv.writer(csvfile, delimiter=',')
        writer.writerow(param_names)
        for row in range(rows):
            writer.writerow([f"{s:.3}" for s in sigmas[row]])


if __name__ == "__main__":
    main()