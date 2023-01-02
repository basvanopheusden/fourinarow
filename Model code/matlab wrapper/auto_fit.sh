#!/bin/bash
#SBATCH --nodes=1
#SBATCH --cpus-per-task=16
#SBATCH --array=0-499
#SBATCH --time=12:00:00
#SBATCH --mem=2GB
#SBATCH --job-name=fourinarow
#SBATCH --mail-type=ALL
#SBATCH --mail-user=svo213@nyu.edu
#SBATCH --output=4inarow_%j.out

player=$((${SLURM_ARRAY_TASK_ID}/5+1))
group=$((${SLURM_ARRAY_TASK_ID}%5+1))
#group=0
direc=$SCRATCH/fourinarowData/peak/splits
codedirec=$SCRATCH/fourinarow/Model\ code/matlab\ wrapper

module purge
module load matlab/2020b

echo $player $group

echo "addpath(genpath('${codedirec}')); cross_val($player,$group,'${direc}'); exit;" | matlab -nodisplay

echo "Done"

