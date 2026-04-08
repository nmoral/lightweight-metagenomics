#!/usr/bin/env python3
"""
Benchmark report generator.
Reads all JSON benchmark results from a directory and generates a comparison report.

Usage:
    python benchmark_report.py benchmarks/results/2026-04-08
"""

import json
import sys
import os
import argparse
from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


def load_results(directory: Path) -> pd.DataFrame:
    """Load all JSON benchmark results from a directory."""
    results = []

    json_files = sorted(directory.glob("*.json"))
    if not json_files:
        print(f"No JSON files found in {directory}")
        sys.exit(1)

    for filepath in json_files:
        with open(filepath) as f:
            data = json.load(f)

        # Extract config from filename
        # Format: HH-MM-SS_mode{MODE}_type{TYPE}_k{KMER_SIZE}.json
        name = filepath.stem
        parts = name.split("_")
        config = "_".join(parts)  # remove timestamp

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


def generate_report(directory: Path) -> None:
    """Generate HTML and PNG comparison report from benchmark results."""

    df = load_results(directory)

    output_dir = directory
    output_dir.mkdir(parents=True, exist_ok=True)

    # --- Bar chart ---
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
    ax.set_title(f"Benchmark comparison — {directory.name}")
    ax.yaxis.set_major_formatter(ticker.FuncFormatter(
        lambda x, _: f"{x:,.0f} ns"
    ))
    ax.legend(title="Configuration", bbox_to_anchor=(1.05, 1), loc="upper left")
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()

    chart_path = output_dir / "comparison.png"
    plt.savefig(chart_path, dpi=150)
    print(f"Chart saved: {chart_path}")

    # --- Summary table ---
    summary = df.groupby(["benchmark", "config"])["time_ns"].mean().reset_index()
    summary["time_ns"] = summary["time_ns"].round(0).astype(int)
    summary = summary.rename(columns={"time_ns": "mean_time_ns"})

    # --- HTML report ---
    html_path = output_dir / "report.html"

    pivot_html = pivot.round(0).astype(int).to_html(
        classes="table",
        border=0
    )

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
    </style>
</head>
<body>
    <h1>Benchmark report</h1>

    <div class="meta">
        <strong>Date :</strong> {directory.name}<br>
        <strong>Files :</strong>
        <span class="files">{", ".join(f.name for f in sorted(directory.glob("*.json")))}</span>
    </div>

    <h2>Results (mean time in ns)</h2>
    {pivot_html}

    <h2>Comparison chart</h2>
    <img src="comparison.png" alt="Benchmark comparison chart">

    <h2>Raw data</h2>
    {summary.to_html(classes="table", border=0, index=False)}
</body>
</html>
"""

    with open(html_path, "w") as f:
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
        help="Path to directory containing JSON benchmark results"
    )
    args = parser.parse_args()

    if not args.directory.exists():
        print(f"Directory not found: {args.directory}")
        sys.exit(1)

    generate_report(args.directory)


if __name__ == "__main__":
    main()