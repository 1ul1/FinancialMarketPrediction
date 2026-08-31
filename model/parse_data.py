import os
import sys
import ssl
import json
import ctypes
from datetime import date
from urllib.request import urlopen

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

class Company(ctypes.Structure):
    _fields_ = [
        ("count", ctypes.c_int),
        ("samples", ctypes.POINTER(Sample)),
    ]

    def __init__(self, ticker: str = "@", count: int = 0, results: list[dict[str, float]] = None):
        super().__init__()
        
        self.ticker = ticker
        self.count = count
        self.samples = None if count == 0 else (Sample * count)()

        for i, row in enumerate(results):
            self.samples[i] = Sample(row["v"], row["vw"], row["o"], row["c"], row["h"], row["l"], row["t"], row["n"])

class Weights(ctypes.Structure):
    _fields_ = [
        ("len_weights", ctypes.c_int),
        ("weights", ctypes.POINTER(ctypes.c_double)),
        ("len_bias", ctypes.c_int),
        ("bias", ctypes.POINTER(ctypes.c_double)),
        ("means", ctypes.POINTER(ctypes.c_double)),
        ("standard_deviations", ctypes.POINTER(ctypes.c_double)),
    ]

    def __init__(self,
        len_weights = 0,
        len_bias: int = 0, 
        weights: list[float] = None,
        bias: list[float] = None,
        means: list[float] = None,
        standard_deviations: list[float] = None
    ):
        super().__init__()
        
        assert (len_weights > 0 and len_bias > 0)
        self.len_weights = len_weights
        self.weights = (ctypes.c_double * len_weights)()
        self.len_bias = len_bias
        self.bias = (ctypes.c_double * len_bias)()
        self.means = (ctypes.c_double * (len_weights // len_bias))()
        self.standard_deviations = (ctypes.c_double * (len_weights // len_bias))()

        if not weights or not bias or not means or not standard_deviations:
            
            for i in range(len_weights):
                self.weights[i] = 0
            for i in range(len_bias):
                self.bias[i] = 0
            for i in range(len_weights // len_bias):
                self.means[i] = 0
                self.standard_deviations[i] = 0

        else:
            
            for i, r in enumerate(weights):
                self.weights[i] = r
            for i, r in enumerate(bias):
                self.bias[i] = r
            for i, r in enumerate(means):
                self.means[i] = r
            for i, r in enumerate(standard_deviations):
                self.standard_deviations[i] = r
            

class Day(ctypes.Structure):
    _fields_ = [
        ("bias", ctypes.c_double),
        ("sd", ctypes.c_double),
        ("expected_return", ctypes.c_double),
        ("expected_price", ctypes.c_double),
        ("starting_price", ctypes.c_double),
        ("forecast_strength", ctypes.c_double),
    ]

    def __init__(self, bias = 0, sd = 0, expected_return = 0, expected_price = 0, starting_price = 0, forecast_strength = 0):
        super().__init__()

        self.bias = bias
        self.sd = sd
        self.expected_return = expected_return
        self.expected_price = expected_price
        self.starting_price = starting_price
        self.forecast_strength = forecast_strength

class Prediction(ctypes.Structure):
    _fields_ = [
        ("today", ctypes.c_int),
        ("len_days", ctypes.c_int),
        ("days", ctypes.POINTER(Day)),
    ]

    def __init__(self, len_days):
        super().__init__()

        self.today = 0
        self.len_days = len_days
        self.days = (Day * len_days)()

    def print(self):
        print(f"Today: {self.today}\n")

        for i in range(self.len_days):
            print(f"Prediction Layer {i + 1}\n")
            print(
                f"bias {self.days[i].bias}\nsd {self.days[i].sd}\nexpected_return {self.days[i].expected_return}"
                +
                f"\nexpected_price {self.days[i].expected_price}\nstarting_price {self.days[i].starting_price}\nforecast_strength {self.days[i].forecast_strength}\n"
            )

lib = ctypes.CDLL("./build/libmodel.dylib")

lib.start.argtypes = [Company, Company, Weights, ctypes.POINTER(Prediction)]
lib.start.restype = None

lib.get_nr_features.argtypes = []
lib.get_nr_features.restype = ctypes.c_int

lib.get_nr_models.argtypes = []
lib.get_nr_models.restype = ctypes.c_int

API_KEY = os.environ.get("MASSIVE_KEY")

RESET_WEIGHTS = 1

def request(ticker) -> Prediction:

    ticker = ticker.upper()
    
    until: str = str(sys.argv[2]) if len(sys.argv) == 3 else str(date.today())

    if RESET_WEIGHTS == 0:
        weights = Weights(lib.get_nr_features() * lib.get_nr_models(), lib.get_nr_models())
    else:
        file = open("./model_weights", "r")
        weights_data, bias_data = file.readline(), file.readline()
        means_data, standard_deviations_data = file.readline(), file.readline()

        weights_data = [float(value) for value in weights_data.split("_")[:-1:]]
        bias_data = [float(value) for value in bias_data.split("_")[:-1:]]
        means_data = [float(value) for value in means_data.split("_")[:-1:]]
        standard_deviations_data = [float(value) for value in standard_deviations_data.split("_")[:-1:]]

        weights = Weights(
            len(weights_data), len(bias_data), weights_data, bias_data, means_data, standard_deviations_data
        )
        
    url = (
        'https://api.massive.com/v2/aggs/ticker/'
        + ticker +
        '/range/1/day/2023-06-23/'
        + until +
        '?adjusted=true&sort=asc&limit=1000&apiKey='
        + API_KEY
    )
    url_market = (
        'https://api.massive.com/v2/aggs/ticker/SPY/range/1/day/2023-06-23/'
        + until +
        '?adjusted=true&sort=asc&limit=1000&apiKey='
        + API_KEY
    )

    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    
    response = urlopen(url, context = ctx)
    response_market = urlopen(url_market, context = ctx)
    
    data = json.load(response)
    data_market = json.load(response_market)

    assert data["resultsCount"] == data_market["resultsCount"]

    company = Company(data["ticker"], data["count"], data["results"])
    market = Company(data_market["ticker"], data_market["count"], data_market["results"])

    prediction: Prediction = Prediction(4)
    lib.start(company, market, weights, ctypes.byref(prediction))
    prediction.print()

    return prediction

if __name__ == "__main__":
    request(str(sys.argv[1]))
