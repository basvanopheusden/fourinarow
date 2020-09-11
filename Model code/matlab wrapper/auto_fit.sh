#!/bin/bash
#SBATCH --nodes=1
#SBATCH --array=0-199
#SBATCH --cpus-per-task=20
#SBATCH --time=24:00:00
#SBATCH --mem=2GB
#SBATCH --job-name=fourinarow
#SBATCH --mail-type=ALL
#SBATCH --mail-user=svo213@nyu.edu
#SBATCH --output=4inarow_%j.out

Nplayers=3
player=$((${SLURM_ARRAY_TASK_ID}%$Nplayers+1))
group=$((${SLURM_ARRAY_TASK_ID}/$Nplayers+1))
#group=0
direc=$HOME/fourinarow/Data/ili/splits

module purge
module load matlab/2018a

echo $player $group

echo "addpath(genpath('Code')); cross_val($player,$group,'${direc}'); exit;" | matlab -nodisplay

echo "Done"
