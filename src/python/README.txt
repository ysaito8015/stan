script samplerEval.py :
must be run from cmdstan directory
runs cmdStan repeatedly for model with model.data.R

paramters:  
- model:  compiled stan model file
- data:   corresponding file model.data.R
- method:  either "nuts" or "ens"
- num_runs:  number of times to run stan

creates 2 output files:
stats_params_<model>_<method>.txt
stats_time_<model>_<method>.txt

these contain the estimated parameters
from each run and the sampling time respectively.

