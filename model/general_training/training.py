import os
import sys
import json
import ctypes

RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
RESET = "\033[0m"

class Sample(ctypes.Structure):
    _fields_ = [
        ("v", ctypes.c_double),
        ("vw", ctypes.c_double),
        ("o", ctypes.c_double),
        ("c", ctypes.c_double),
        ("h", ctypes.c_double),
        ("l", ctypes.c_double),
        ("t", ctypes.c_double),
        ("n", ctypes.c_double),
    ]

    def __init__(self, v = 0, vw = 0, o = 0, c = 0, h = 0, l = 0, t = 0, n = 0):
        global COUNTER_SAMPLE
        super().__init__()
        
        self.v = v
        self.vw = vw
        self.o = o
        self.c = c
        self.h = h
        self.l = l
        self.t = t
        self.n = n
        
        if any(field <= 0 for field in (v, vw, o, c, h, l, t, n)):
            print("Not good, ruins training data")
            print(f"{v} {vw} {o} {c} {h} {l} {t} {n}")
        COUNTER_SAMPLE += 1

class Company(ctypes.Structure):
    _fields_ = [
        ("count", ctypes.c_int),
        ("samples", ctypes.POINTER(Sample)),
    ]

    def __init__(self, ticker: str = "@", count: int = 0, results: list[dict[str, float]] = None):
        global COUNTER_COMPANY
        super().__init__()
        
        self.ticker = ticker
        self.count = count
        self.samples = None if count == 0 else (Sample * count)()

        for i, row in enumerate(results):
            self.samples[i] = Sample(row["v"], row["vw"], row["o"], row["c"], row["h"], row["l"], row["t"], row["n"])

        COUNTER_COMPANY += 1

class Companies(ctypes.Structure):
    _fields_ = [
        ("len_companies", ctypes.c_int),
        ("companies", ctypes.POINTER(Company))
    ]

    def __init__(self, len_companies: int, files: list[str]):
        super().__init__()
        
        self.len_companies = len_companies
        self.companies = None if len_companies == 0 else (Company * len_companies)()

        for i, f in enumerate(files):
            with open(f"../../../stocks/{f}", "r") as file:
                data = json.load(file)
                try:
                    self.companies[i] = Company(data["ticker"], data["count"], data["results"])
                    assert data["count"] == 501
                except:
                    print("Invalid file or size")
                    print(data)
                    print(f)

class Weights(ctypes.Structure):
    _fields_ = [
        ("len_weights", ctypes.c_int),
        ("weights", ctypes.POINTER(ctypes.c_double)),
        ("len_bias", ctypes.c_int),
        ("bias", ctypes.POINTER(ctypes.c_double)),
        ("means", ctypes.POINTER(ctypes.c_double)),
        ("standard_deviations", ctypes.POINTER(ctypes.c_double)),
    ]

    def __init__(self, len_weights = 0, results: list[float] = None, len_bias: int = 0, results_bias: list[float] = None):
        super().__init__()
        
        assert (len_weights > 0 and len_bias > 0)
        self.len_weights = len_weights
        self.weights = (ctypes.c_double * len_weights)()
        self.len_bias = len_bias
        self.bias = (ctypes.c_double * len_bias)()
        self.means = (ctypes.c_double * (len_weights // len_bias))()
        self.standard_deviations = (ctypes.c_double * (len_weights // len_bias))()

        for i, r in enumerate(results):
            self.weights[i] = r
            # print(f"W {i} - {r}")

        for i, r in enumerate(results_bias):
            self.bias[i] = r
            # print(f"B {i} - {r}")

        for i in range(len_weights // len_bias):
            self.means[i] = 0
            
        for i in range(len_weights // len_bias):
            self.standard_deviations[i] = 0

    def print(self):
        for i in range(self.len_weights):
            print(GREEN + f"W {i} - " + str(self.weights[i]) + RESET)
        for i in range(self.len_bias):
            print(YELLOW +  f"B {i} - " + str(self.bias[i]) + RESET)

    def static_save(self):
        with open("./model_weights", "w") as file:
            file.write("_".join(format(self.weights[i], ".17g") for i in range(self.len_weights)))
            file.write("_\n")
            file.write("_".join(format(self.bias[i], ".17g") for i in range(self.len_bias)))
            file.write("_\n")
            file.write("_".join(format(self.means[i], ".17g") for i in range(self.len_weights // self.len_bias)))
            file.write("_\n")
            file.write("_".join(format(self.standard_deviations[i], ".17g") for i in range(self.len_weights // self.len_bias)))
            file.write("_\n")
            print("Weights Saved!\n")

            for i in range(self.len_weights // self.len_bias):
                self.means[i] = 0
                self.standard_deviations[i] = 0

MARKET: Company = None
COMPANIES: Companies = None
UNTRAINED_COMPANIES: Companies = None
WEIGHTS: Weights = None

COUNTER_SAMPLE: int = 0
COUNTER_COMPANY: int = 0

lib = ctypes.CDLL("./libtraining.dylib")

lib.get_nr_features.argtypes = []
lib.get_nr_features.restype = ctypes.c_int

lib.start.argtypes = [Companies, Companies, Company, Weights]
lib.start.restype = None

def aux():
    global MARKET, COMPANIES, WEIGHTS
    
    files = os.listdir("../../../stocks")
    
    COMPANIES = Companies(len(files), files)

    files = os.listdir("../../../untrained_stocks")

    UNTRAINED_COMPANIES = Companies(len(files), files)

    with open("../../../market/SPY.json", "r") as file:
        data = json.load(file)
        MARKET = Company(data["ticker"], data["count"], data["results"])

    with open("./model_weights", "r") as file:
        weights, bias = file.readline(), file.readline()

        weights = [float(value) for value in weights.split("_")[:-1:]]
        bias = [float(value) for value in bias.split("_")[:-1:]]
        
        WEIGHTS = Weights(len(weights), weights, len(bias), bias)

    if not (COMPANIES and MARKET and WEIGHTS):
        print("IO failure")
        
    try:
        while True:
            lib.start(COMPANIES, UNTRAINED_COMPANIES, MARKET, WEIGHTS)
            WEIGHTS.static_save()
    except Exception as e:
        print(e)

    if len(sys.argv) > 1 and sys.argv[1] == "verify":
        print(f"COUNTER_SAMPLE: {COUNTER_SAMPLE}________________COUNTER_COMPANY:{COUNTER_COMPANY}")
        return

def reset_weights():
    nr_features = lib.get_nr_features()
    nr_weights: int = nr_features * 4

    with open("./model_weights", "w") as file:
        for _ in range(nr_weights):
            file.write("0_")
        file.write("\n")
        for _ in range(4):
            file.write("0_")
        file.write("\n")
        for _ in range(nr_features):
            file.write("0_")
        file.write("\n")
        for _ in range(nr_features):
            file.write("0_")
        file.write("\n")

def test_py_c_bridge():
    print(lib.get_nr_features())
    
if __name__ == "__main__":
    print("")

    reset_weights()

    if len(sys.argv) > 1 and sys.argv[1] == "reset":
        reset_weights()
        exit(-1)
    elif len(sys.argv) > 1 and sys.argv[1] == "test":
        test_py_c_bridge()
        exit(-1)

    aux()
