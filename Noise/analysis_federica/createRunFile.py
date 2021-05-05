import sys
import os
import csv
import string
import datetime

# Define the parser
import argparse
parser = argparse.ArgumentParser(description="Options to give to the script")
# Positional arguments
parser.add_argument("dataset", type=str, choices=['data', 'MC'], help="Specify if data or Monte Carlo")
# Optional Arguments
parser.add_argument("--run", type=str, default='', choices=[
                                                             '338702',
                                                             '338714',
                                                             '340116',
                                                             '340220',
                                                             '340323',
                                                             '341343'
                                                            ], help="MWGR run in data")

parser.add_argument("--n", type=int, default=255, help="number of .root files per job")
parser.add_argument("--MCprocess", type=str, default='', choices=['Cosmics', 'Zmumu'], help="process in Monte Carlo")
parser.add_argument("--anatype", type=str, default='', choices=['efficiency', 'noise'], help="analysis")
args = parser.parse_args()

#prepare output filename  and option string
if args.dataset == 'data':
   out_filename = 'AnalysedTree_'+args.dataset+'_'+args.run+'_'+args.anatype
   temp = '_'+args.anatype
   option_string = ' "'+args.dataset+'" "'+args.run+'"'
else:
   out_filename = 'AnalysedTree_'+args.dataset+'_'+args.MCprocess+'_'+args.anatype
   temp = '_'+args.anatype
   option_string = ' "'+args.dataset+'" "'+args.MCprocess+'"'

startTime = datetime.datetime.now().strftime("%Y%m%d_%H%M")

# Create target Directory if don't exist
output_folder = '/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/'
code_folder = os.path.dirname(os.path.abspath(__file__))

if not os.path.exists(output_folder+startTime):
    os.mkdir(output_folder+startTime)
    print('Directory '+output_folder+startTime+' created\n')
else:    
    print('Directory '+output_folder+startTime+' already exists\n')

if args.dataset == 'data' and args.run == '338702':
    path = '/lustre/cms/store/user/gmilella/ExpressCosmics/CRAB3_gem_dpg_ntuple_mwgr5_2020_run338702_express/'
if args.dataset == 'data' and args.run == '338714':
    path = '/lustre/cms/store/user/gmilella/ExpressCosmics/CRAB3_gem_dpg_ntuple_mwgr5_2020_run338714_express/210325_201554/0000/'
if args.dataset == 'data' and args.run == '340116':
    path = '/lustre/cms/store/user/gmilella/ExpressCosmics/CRAB3_gem_dpg_ntuple_mwgr2_2021_run340116_express/210304_122949/0000/'
if args.dataset == 'data' and args.run == '340220':
    path = '/lustre/cms/store/user/gmilella/ExpressCosmics/CRAB3_gem_dpg_ntuple_mwgr2_2021_run340220_express/210306_100351/0000/'
if args.dataset == 'data' and args.run == '340323':
    path = '/lustre/cms/store/user/gmilella/ExpressCosmics/CRAB3_gem_dpg_ntuple_mwgr2_2021_run340323_express/210323_122615/0000/'
if args.dataset == 'data' and args.run == '341343':
    path = '/lustre/cms/store/user/fsimone/MWGR3_Run_341343/'
if args.dataset == 'MC' and args.MCprocess == 'Cosmics':
    path = ''
if args.dataset == 'MC' and args.MCprocess == 'Zmumu':
    path = ''

#generating the list of all .root files in given directory and subdirectories
fileList = []
for r, d, f in os.walk(path): # r=root, d=directories, f = files
    for file in f:
        if '.root' in file:
            fileList.append(os.path.join(r, file))

#prepare final script
final_script_path = output_folder+startTime+"/submit_analysis_"+startTime+".sh"
final_script = open(final_script_path, "w")
final_script.write("#!/bin/bash\n")
final_script.write("chmod 777 -R "+output_folder+startTime+"/*\n")
final_script.write("chmod 777 -R "+code_folder+"/*\n")
final_script.write("cd "+output_folder+startTime+"\n")

#loop to generate one .cpp+executable+batch system conf file for each group of "n" files
n_chunk = len(fileList)//args.n
print('using ntuples in '+path)
print('Number of files is {0:2d}'.format(len(fileList)))
print('Number of jobs is {0:2d}'.format(n_chunk+1))
for file_index in range(n_chunk+1):
      chunk = '' 
      for idx, l in enumerate(fileList):
         if idx < args.n*(file_index+1) and idx >= args.n*file_index:
             l = l.rstrip()
             l = '        chain->AddFile("{}");\n'.format(l)
             chunk = chunk + l

      #analysis.cpp template
      with open("templates/Analysis_template.cpp", "r") as in_file:
          buf = in_file.readlines()

      cpp_filename = "Analysis_"+args.dataset+"_"+args.run+args.MCprocess+"_"+args.anatype+"_chunk"+str(file_index)+".cpp"
      with open(cpp_filename, "w") as out_file:
          for lb in buf:
              if lb == '    //AddFile\n':
                  #write group of files
                  out_file.write(chunk)
              elif lb == '    //OutFile\n':
                  #write output file name
                  out_file.write('        fileout = "'+out_filename+str(file_index)+'.root";\n')
              else: out_file.write(lb)

      #executable template
      with open("templates/launch_analysis_template.job", "r") as launch_infile:
          buf2 = launch_infile.readlines()

      launch_filename = "launch_analysis_"+args.dataset+"_"+args.run+args.MCprocess+"_"+args.anatype+"_"+str(file_index)+".job"
      with open(output_folder+startTime+"/"+launch_filename, "w") as launch_outfile:
          for lb2 in buf2:
              if lb2 == "#compile\n":
                  launch_outfile.write("cd "+output_folder+startTime+"\n")
                  launch_outfile.write("g++ -I $ROOTSYS/include "+code_folder+"/"+cpp_filename+" `root-config --glibs` `root-config --libs` `root-config --cflags` -lTMVA -L $ROOTSYS/lib -o executable"+str(file_index)+"\n")
              elif lb2 == "#execute\n":
                  launch_outfile.write('./executable'+str(file_index)+option_string+'\n')
              else: launch_outfile.write(lb2)

      #myCondor template
      with open("templates/my_HTCondor_template.job", "r") as myCondor_infile:
          buf3 = myCondor_infile.readlines()

      condor_filename = "my_HTCondor_"+args.dataset+"_"+args.run+args.MCprocess+"_"+args.anatype+"_"+str(file_index)+".job"
      with open(output_folder+startTime+"/"+condor_filename, "w") as myCondor_outfile:
          for lb3 in buf3:
              if lb3 == "Executable = launch_analysis_template.job\n":
                  myCondor_outfile.write("Executable = "+launch_filename+"\n")
              else: myCondor_outfile.write(lb3)

      #add lines to final script
      final_script.write("echo condor_submit "+condor_filename+" -name ettore\n")
      final_script.write("condor_submit "+condor_filename+" -name ettore\n")

final_script.write("cd "+code_folder+"\n")
final_script.close()
print("to submit analysis to batch system do:\nsource "+final_script_path+"\n")
