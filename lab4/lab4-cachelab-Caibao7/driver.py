#!/usr//bin/python2
#
# driver.py - The driver tests the correctness of the student's cache
#     simulator and the correctness and performance of their transpose
#     function. It uses ./test-csim to check the correctness of the
#     simulator and it runs ./test-trans on three different sized
#     matrices (48x48, 96x96, and 93x99) to test the correctness and
#     performance of the transpose function.
#
import subprocess;
import re;
import os;
import sys;
import optparse;

#
# computeMissScore - compute the score depending on the number of
# cache misses
#
def computeMissScore(miss, lower, upper, full_score):
    if miss <= lower:
        return full_score
    if miss >= upper: 
        return 0

    score = (miss - lower) * 1.0 
    range = (upper- lower) * 1.0
    return round((1 - score / range) * full_score, 1)

#
# main - Main function
#
def main():

    # Configure maxscores here
    maxscore= {};
    maxscore['csim'] = 39
    maxscore['trans48'] = 15
    maxscore['trans96'] = 15
    honorpart = [2,2,9]

    # Parse the command line arguments 
    p = optparse.OptionParser()
    p.add_option("-A", action="store_true", dest="autograde", 
                 help="emit autoresult string for Autolab");
    opts, args = p.parse_args()
    autograde = opts.autograde

    # Check the correctness of the cache simulator
    if not autograde:
        print("Part A: Testing cache simulator")
        print("the following are original points, which will be transformed into the final points")
        print("Running ./test-csim")
    p = subprocess.Popen("./test-csim", 
                         shell=True, stdout=subprocess.PIPE)
    stdout_data = p.communicate()[0]

    # Emit the output from test-csim
    stdout_data = re.split('\n', stdout_data.decode())
    for line in stdout_data:
        if re.match("TEST_CSIM_RESULTS", line):
            resultsim = re.findall(r'(\d+)', line)
        else:
            if not autograde:
                print(line)
    csim_cscore = int(resultsim[0])

    if not autograde:
        print(f"part A final points: {csim_cscore}")
        # Check the correctness and performance of the transpose function
        # 48x48 transpose
        print("Part B: Testing transpose function")
        print("Running ./test-trans -M 48 -N 48")
    p = subprocess.Popen("./test-trans -M 48 -N 48 | grep TEST_TRANS_RESULTS", 
                         shell=True, stdout=subprocess.PIPE)
    stdout_data = p.communicate()[0].decode()
    result48 = re.findall(r'(\d+)', stdout_data)
    
    # 96x96 transpose
    if not autograde:
        print("Running ./test-trans -M 96 -N 96")
    p = subprocess.Popen("./test-trans -M 96 -N 96 | grep TEST_TRANS_RESULTS", 
                         shell=True, stdout=subprocess.PIPE)
    stdout_data = p.communicate()[0].decode()
    result96 = re.findall(r'(\d+)', stdout_data)
    if not autograde:
        print("Running heat-sim")
    p = subprocess.Popen("cd heat-sim\n./test-heat | grep TEST_HEAT_SCORES", 
                         shell=True, stdout=subprocess.PIPE)
    stdout_data = p.communicate()[0].decode()
    result_heat = re.findall(r'(\d+)', stdout_data)

    # Compute the scores for each step
    miss48 = int(result48[1])
    miss96 = int(result96[1])
    heat_score = int(result_heat[0])
    trans48_score = computeMissScore(miss48, 500, 800, maxscore['trans48']) * int(result48[0])
    if miss48 < 450:
        trans48_score += honorpart[0]
    trans96_score = computeMissScore(miss96, 2200, 3000, maxscore['trans96']) * int(result96[0])
    if miss96 < 1900:
        trans96_score += honorpart[1]

    total_score = csim_cscore + trans48_score + trans96_score + heat_score

    # Summarize the results
    if not autograde:
        print("\nCache Lab summary:")
        print("%-22s%8s%10s%12s" % ("", "Points", "Max pts", "Misses"))
        print("%-22s%8.1f%10d" % ("Csim correctness", csim_cscore, maxscore['csim']))

    misses = str(miss48)
    if miss48 == 2**31-1 :
        misses = "invalid"

    if not autograde:
        print("%-22s%8.1f%10d%12s" % ("Trans perf 48x48", trans48_score, maxscore['trans48'] + honorpart[0], misses))

    misses = str(miss96)
    if miss96 == 2**31-1 :
        misses = "invalid"

    if not autograde:
        print("%-22s%8.1f%10d%12s" % ("Trans perf 96x96", trans96_score, maxscore['trans96'] + honorpart[1], misses))

        print("%-22s%8.1f%10d%12s" % ("Heat Simulator perf C", heat_score, honorpart[2], ""))
    
        print("%22s%8.1f%10d" % ("Total points", total_score, maxscore['csim'] +  maxscore['trans48'] +  maxscore['trans96'] + honorpart[0] + honorpart[1] + honorpart[2]))
    
    # Emit autoresult string for Autolab if called with -A option
    if autograde:
        print(f"{csim_cscore}_{trans48_score}_{trans96_score}_{heat_score}")
    
    
# execute main only if called as a script
if __name__ == "__main__":
    main()

