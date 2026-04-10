#!/usr/bin/env python3
"""
Benchmark report generator.
Reads all JSON benchmark results and massif memory files from a directory
and generates a unified comparison report.

Usage:
    python benchmark_report.py benchmarks/results/2026-04-08
"""

import json
import sys
import re
import argparse
from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


# --- Loaders ---

def load_time_results(directory: Path) -> pd.DataFrame:
    """Load all JSON benchmark results from a directory."""
    results = []

    json_files = sorted(directory.glob("*.json"))
    if not json_files:
        return pd.DataFrame()

    for filepath in json_files:
        with open(filepath) as f:
            data = json.load(f)

        name = filepath.stem
        config = "_".join(name.split("_"))

        for benchmark in data.get("benchmarks", []):
            results.append({
                "config":     config,
                "benchmark":  benchmark["name"],
                "time_ns":    benchmark["real_time"],
                "cpu_ns":     benchmark["cpu_time"],
                "iterations": benchmark["iterations"],
                "file":       filepath.name,
            })

    return pd.DataFrame(results)


def parse_massif(filepath: Path) -> tuple[str, list[dict]]:
    """
    Parse a .massif file and return (config_name, list of snapshots).
    Each snapshot: {"snapshot": int, "time": int, "mem_bytes": int}
    """
    config = filepath.stem  # memory_mode1_type0_k31

    snapshots = []
    current = {}

    with open(filepath) as f:
        for line in f:
            line = line.strip()
            if line.startswith("snapshot="):
                if current:
                    snapshots.append(current)
                current = {"snapshot": int(line.split("=")[1])}
            elif line.startswith("time="):
                current["time"] = int(line.split("=")[1])
            elif line.startswith("mem_heap_B="):
                current["mem_bytes"] = int(line.split("=")[1])

    if current:
        snapshots.append(current)

    return config, snapshots


def load_memory_results(directory: Path) -> dict[str, list[dict]]:
    """Load all .massif files from a directory. Returns {config: snapshots}."""
    results = {}
    for filepath in sorted(directory.glob("*.massif")):
        config, snapshots = parse_massif(filepath)
        results[config] = snapshots
    return results


def peak_mb(snapshots: list[dict]) -> float:
    return max(s["mem_bytes"] for s in snapshots) / (1024 * 1024)


# --- Chart generators ---

def generate_time_chart(df: pd.DataFrame, output_path: Path, title: str) -> None:
    fig, ax = plt.subplots(figsize=(14, 7))

    pivot = df.pivot_table(
        index="benchmark",
        columns="config",
        values="time_ns",
        aggfunc="mean"
    )

    pivot.plot(kind="bar", ax=ax, logy=True)

    ax.set_ylabel("Time (ns) — log scale")
    ax.set_xlabel("")
    ax.set_title(f"Benchmark comparison — {title}")
    ax.yaxis.set_major_formatter(ticker.FuncFormatter(
        lambda x, _: f"{x:,.0f} ns"
    ))
    ax.legend(title="Configuration", bbox_to_anchor=(1.05, 1), loc="upper left")
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"Time chart saved: {output_path}")


def generate_memory_peak_chart(memory_data: dict, output_path: Path, title: str) -> None:
    configs = list(memory_data.keys())
    peaks   = [peak_mb(memory_data[c]) for c in configs]

    fig, ax = plt.subplots(figsize=(10, 5))
    bars = ax.bar(configs, peaks, color="#89b4fa")
    ax.set_ylabel("Peak memory (MB)")
    ax.set_title(f"Peak memory usage — {title}")
    ax.set_ylim(0, max(peaks) * 1.2)
    for bar, val in zip(bars, peaks):
        ax.text(
            bar.get_x() + bar.get_width() / 2,
            bar.get_height() + 0.05,
            f"{val:.2f} MB",
            ha="center", va="bottom", fontsize=9
        )
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"Memory peak chart saved: {output_path}")


def generate_memory_curve_chart(memory_data: dict, output_path: Path, title: str) -> None:
    fig, ax = plt.subplots(figsize=(14, 6))

    for config, snapshots in memory_data.items():
        xs = list(range(len(snapshots)))
        ys = [s["mem_bytes"] / (1024 * 1024) for s in snapshots]
        ax.plot(xs, ys, label=config, marker=".")

    ax.set_xlabel("Snapshot index")
    ax.set_ylabel("Memory (MB)")
    ax.set_title(f"Memory usage over time — {title}")
    ax.legend(title="Configuration", bbox_to_anchor=(1.05, 1), loc="upper left")
    plt.tight_layout()
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"Memory curve chart saved: {output_path}")


# --- HTML sections ---

def time_section(df: pd.DataFrame) -> str:
    if df.empty:
        return "<p>No time benchmark data found.</p>"

    pivot = df.pivot_table(
        index="benchmark",
        columns="config",
        values="time_ns",
        aggfunc="mean"
    )
    pivot_html = pivot.round(0).astype(int).to_html(classes="table", border=0)

    summary = (
        df.groupby(["benchmark", "config"])["time_ns"]
        .mean()
        .reset_index()
        .rename(columns={"time_ns": "mean_time_ns"})
    )
    summary["mean_time_ns"] = summary["mean_time_ns"].round(0).astype(int)

    return f"""
    <h2>Time results (mean in ns)</h2>
    {pivot_html}
    <h2>Time comparison chart</h2>
    <img src="comparison_time.png" alt="Time benchmark comparison chart">
    <h2>Raw time data</h2>
    {summary.to_html(classes="table", border=0, index=False)}
    """


def memory_section(memory_data: dict) -> str:
    if not memory_data:
        return "<p>No memory data found (.massif files).</p>"

    rows = "".join(
        f"<tr><td>{config}</td><td>{peak_mb(snaps):.2f} MB</td></tr>"
        for config, snaps in sorted(memory_data.items())
    )
    table = f"""
    <table class="table">
        <thead><tr><th>Configuration</th><th>Peak memory</th></tr></thead>
        <tbody>{rows}</tbody>
    </table>
    """

    return f"""
    <h2>Memory results (peak RSS)</h2>
    {table}
    <h2>Peak memory chart</h2>
    <img src="comparison_memory_peak.png" alt="Memory peak chart">
    <h2>Memory usage over time</h2>
    <img src="comparison_memory_curve.png" alt="Memory curve chart">
    """


# --- Main ---

def generate_report(directory: Path) -> None:
    df           = load_time_results(directory)
    memory_data  = load_memory_results(directory)

    if df.empty and not memory_data:
        print("No data found (no .json or .massif files).")
        sys.exit(1)

    directory.mkdir(parents=True, exist_ok=True)

    json_files = ", ".join(f.name for f in sorted(directory.glob("*.json")))
    massif_files = ", ".join(f.name for f in sorted(directory.glob("*.massif")))

    if not df.empty:
        generate_time_chart(df, directory / "comparison_time.png", directory.name)

    if memory_data:
        generate_memory_peak_chart(memory_data, directory / "comparison_memory_peak.png", directory.name)
        generate_memory_curve_chart(memory_data, directory / "comparison_memory_curve.png", directory.name)

    html = f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Benchmark report — {directory.name}</title>
    <style>
        body {{
            font-family: monospace;
            max-width: 1200px;
            margin: 40px auto;
            padding: 0 20px;
            background: #1e1e2e;
            color: #cdd6f4;
        }}
        h1, h2 {{ color: #89b4fa; }}
        .table {{
            border-collapse: collapse;
            width: 100%;
            margin: 20px 0;
        }}
        .table th, .table td {{
            border: 1px solid #313244;
            padding: 8px 16px;
            text-align: right;
        }}
        .table th {{
            background: #313244;
            color: #89b4fa;
        }}
        .table tr:nth-child(even) {{ background: #181825; }}
        img {{ max-width: 100%; margin: 20px 0; border: 1px solid #313244; }}
        .meta {{
            background: #181825;
            padding: 16px;
            border-left: 4px solid #89b4fa;
            margin: 20px 0;
        }}
        .files {{ color: #a6e3a1; font-size: 0.9em; }}
        .section-divider {{
            border: none;
            border-top: 1px solid #313244;
            margin: 40px 0;
        }}
    </style>
</head>
<body>
    <h1>Benchmark report</h1>

    <div class="meta">
        <strong>Date :</strong> {directory.name}<br>
        <strong>Time files :</strong> <span class="files">{json_files or "none"}</span><br>
        <strong>Memory files :</strong> <span class="files">{massif_files or "none"}</span>
    </div>

    {time_section(df)}

    <hr class="section-divider">

    {memory_section(memory_data)}
</body>
</html>
"""

    html_path = directory / "report.html"
    with open(html_path, "w", encoding="utf-8") as f:
        f.write(html)

    print(f"Report saved: {html_path}")
    print(f"\nOpen with: xdg-open {html_path}")


def main():
    parser = argparse.ArgumentParser(
        description="Generate benchmark comparison report from a results directory."
    )
    parser.add_argument(
        "directory",
        type=Path,
        help="Path to directory containing JSON and/or .massif benchmark results"
    )
    args = parser.parse_args()

    if not args.directory.exists():
        print(f"Directory not found: {args.directory}")
        sys.exit(1)

    generate_report(args.directory)


if __name__ == "__main__":
    main()