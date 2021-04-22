
import sys
import os
import csv
import string
import datetime

# Define the parser
import argparse
parser = argparse.ArgumentParser(description="Options to give to the script")
# Optional Arguments
parser.add_argument("--inputFolder", type=str, default='', help="input folder path")
parser.add_argument("--n", type=int, default=10, help="number of .root files per job")
args = parser.parse_args()

startTime = datetime.datetime.now().strftime("%Y%m%d_%H%M")

# Create target Directory if don't exist
output_path = '/lustre/cms/store/user/fsimone/MuonDPGNtuples_output/'
if not os.path.exists(output_path+startTime):
    os.mkdir(output_path+startTime)
    print('Directory '+output_path+startTime+' created\n')
else:    
    print('Directory '+output_path+startTime+' already exists\n')
output_folder=output_path+startTime

# Set code folder
code_folder = '/lustrehome/fsimone/Gabriele_MCprod/CMSSW_11_1_2_patch2/src/MuDPGAnalysis/MuonDPGNtuples'

#path containing input data
path = args.inputFolder
#generating the list of all .root files in given directory and subdirectories
fileList = []
for r, d, f in os.walk(path): # r=root, d=directories, f = files
    for file in f:
        if '.root' in file:
            fileList.append(os.path.join(r, file))

#prepare final script
final_script_path = output_folder+"/submit_analysis_"+startTime+".sh"
final_script = open(final_script_path, "w")
final_script.write("#!/bin/bash\n")
final_script.write("chmod 777 -R "+output_folder+"/*\n")
final_script.write("chmod 777 -R "+code_folder+"/*\n")
final_script.write("cd "+output_folder+"\n")

#loop to generate one muDpgNtuples_cfg.py+executable+batch system conf file for each group of "n" files
n_chunk = len(fileList)//args.n
print('using ntuples in '+path)
print('Number of files is {0:2d}'.format(len(fileList)))
print('Number of jobs is {0:2d}'.format(n_chunk+1))
for file_index in range(n_chunk+1):
      chunk = '' 
      for idx, l in enumerate(fileList):
         if idx < args.n*(file_index+1) and idx >= args.n*file_index:
             l = l.rstrip()
             l = '        "file://{}",\n'.format(l)
             chunk = chunk + l

      #open template_muDpgNtuples_cfg.py
      with open("template_muDpgNtuples_cfg.py", "r") as in_file:
          buf = in_file.readlines()

      cmsrun_filename = output_folder+"/muDpgNtuples_cfg_chunk"+str(file_index)+".py"
      with open(cmsrun_filename, "w") as out_file:
          for lb in buf:
              if lb == '#ADD INPUT FILES HERE\n':
                  #write group of files
                  out_file.write(chunk)
              elif lb == '#ADD OUTPUT PATH HERE\n':
                  #write output file name
                  out_file.write('"'+output_folder+'/muDpgNtuples_output_chunk_'+str(file_index)+'.root",\n')
              else: out_file.write(lb)

      #executable template
      with open("template_launch_ntupliser.job", "r") as launch_infile:
          buf2 = launch_infile.readlines()

      launch_filename = output_folder+"/launch_ntupliser_chunk_"+str(file_index)+".job"
      with open(launch_filename, "w") as launch_outfile:
          for lb2 in buf2:
              if lb2 == "#CMSRUN\n":
                  launch_outfile.write('cd '+output_folder+'\n')
                  launch_outfile.write('cmsRun '+cmsrun_filename+'\n')
              else: launch_outfile.write(lb2)

      #myCondor template
      with open("template_my_HTCondor_MuDPGNtuple.job", "r") as myCondor_infile:
          buf3 = myCondor_infile.readlines()

      condor_filename = output_folder+"/my_HTCondor_MuDPGNtuple_chunk_"+str(file_index)+".job"
      with open(condor_filename, "w") as myCondor_outfile:
          for lb3 in buf3:
              if lb3 == "Executable = \n":
                  myCondor_outfile.write("Executable = "+launch_filename+"\n")
              else: myCondor_outfile.write(lb3)

      #add lines to final script
      final_script.write("echo condor_submit "+condor_filename+" -name ettore\n")
      final_script.write("condor_submit "+condor_filename+" -name ettore\n")
     
final_script.write("cd "+code_folder+"\n")
final_script.close()

print("to submit analysis to batch system do:\nsource "+final_script_path+"\n")
