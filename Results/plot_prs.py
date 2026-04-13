import re
import csv
from pathlib import Path
import matplotlib.pyplot as plt


def parse_prs(filepath):
    lines = Path(filepath).read_text(encoding="latin1").splitlines()

    type_line = next(line for line in lines if line.startswith("TYPE"))
    bias_line = next(line for line in lines if line.startswith("BIAS1"))
    cal_line  = next(line for line in lines if line.startswith("Calibration\t"))
    data_lines = [line for line in lines if line.startswith("Data:")]

    if not data_lines:
        raise ValueError("No Data: rows found in PRS file.")

    data_line = data_lines[-1]   # use last measurement row

    type_parts = type_line.split("\t")
    bias_parts = bias_line.split("\t")
    cal_parts  = cal_line.split("\t")
    data_parts = data_line.split("\t")

    labels = type_parts[5:]
    bias   = bias_parts[5:]
    cal    = cal_parts[5:]
    raw    = data_parts[5:]

    timetic = float(data_parts[2])

    detector_data = {}
    for i, label in enumerate(labels):
        try:
            raw_count = float(raw[i]) if i < len(raw) and raw[i] else 0.0
            leak_rate = float(bias[i]) if i < len(bias) and bias[i] else 0.0
            cf        = float(cal[i]) if i < len(cal) and cal[i] else 1.0
        except ValueError:
            continue

        signal = (raw_count - timetic * leak_rate) * cf
        detector_data[label] = signal

    return detector_data


def extract_profile(detector_data, prefix="X", spacing_cm=0.5, normalize=True):
    profile = []

    for label, value in detector_data.items():
        if re.fullmatch(rf"{prefix}\d+", label):
            idx = int(re.findall(r"\d+", label)[0])
            profile.append((idx, value))

    if not profile:
        raise ValueError(f"No detectors found for prefix '{prefix}'")

    profile.sort(key=lambda x: x[0])

    # use middle detector as zero position
    n = len(profile)
    center_idx = profile[n // 2][0]

    profile_cm = [((idx - center_idx) * spacing_cm, val) for idx, val in profile]

    if normalize:
        max_val = max(v for _, v in profile_cm)
        if max_val != 0:
            profile_cm = [(x, v / max_val) for x, v in profile_cm]

    return profile_cm


def read_csv_profile(filepath, normalize=True, x_unit="mm"):
    x = []
    y = []

    with open(filepath, newline="") as f:
        reader = csv.reader(f)
        for row in reader:
            if len(row) < 2:
                continue
            try:
                xv = float(row[0])
                yv = float(row[3])
                x.append(xv)
                y.append(yv)
            except ValueError:
                continue

    if not x:
        raise ValueError("No numeric data found in CSV file.")

    # convert mm to cm if needed
    if x_unit == "mm":
        x = [v / 10.0 for v in x]

    if normalize:
        ymax = max(y)
        if ymax != 0:
            y = [v / ymax for v in y]

    return list(zip(x, y))


def plot_profiles(prs_profile, csv_profile, title="Measured vs Model Profile"):
    x_prs = [p[0] for p in prs_profile]
    y_prs = [p[1] for p in prs_profile]

    x_csv = [p[0] for p in csv_profile]
    y_csv = [p[1] for p in csv_profile]

    plt.figure(figsize=(9, 5))
    plt.plot(x_prs, y_prs, marker='o', markersize=3, label="Measured (PRS)")
    plt.plot(x_csv, y_csv, label="Model (CSV)")
    plt.xlabel("Position (cm)")
    plt.ylabel("Normalized signal")
    plt.title(title)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    prs_file = "6X_20x20.prs"
    csv_file = "fluence_ex2_20x20profile.csv"

    detector_data = parse_prs(prs_file)

    # choose X or Y depending on which profile you want
    prs_profile = extract_profile(detector_data, prefix="X", spacing_cm=0.5, normalize=True)

    # CSV positions assumed to be in mm
    csv_profile = read_csv_profile(csv_file, normalize=True, x_unit="mm")

    plot_profiles(prs_profile, csv_profile, title="X Profile: PRS vs Head Model")
