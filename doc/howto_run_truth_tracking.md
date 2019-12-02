# How to run truth tracking

## Prerequisites

The framework has to be build with Pythia8 support (`USE_PYTHIA8=ON`) to enable
the fast simulation. `<build>` is used to identify the path to the build
directory.

## Generate a simulation dataset

Generate a simulation dataset with the default signal process and an average
of 50 additional pile-up interactions using the following command:

    <build>/bin/ACTFWGenericFatrasExample \
      --evg-input-type=pythia8 \
      --evg-pileup=50 \
      --output-dir=sim \
      --output-csv=1 \
      --events=10

Setting the output to CSV is necessary since the truth tracking only reads
CSV files at the moment.

## Run the truth tracking

Run the truth tracking tool that reads the simulation output, creates smeared
measurements from the true hits, builds truth tracks, i.e. uses the truth
information to group simulated hits into tracks, and fits them:

    <build>/bin/ActsRecTruthTracks \
      --input-dir=sim \
      --output-dir=rec
