#!/usr/bin/fish
#
# A fish script to run the benchmarks and extract timings from the RDF logs.
# Expects to live alongside the benchmark binaries.

###### CONFIGURATION #####
set THREAD_COUNTS 0 2 4

###### HELPERS ######
function parse_rdf_logs
   argparse -X1 -N1 -- $argv
   awk '
   /Nothing to jit/ { jit_time = 0 }
   /Finished event loop number 0/ {
      match($0, /([[:digit:]]+\.[[:digit:]]+)s CPU, ([[:digit:]]+\.[[:digit:]]+)s elapsed/, arr)
      cpu_time = arr[1]
      wall_time = arr[2]
   }
   END { printf "%s, %s, %s\n", wall_time, cpu_time, jit_time  }' $argv[1]
end

###### FUNCTIONS TO RUN BENCHMARKS #######
function get_data_dir
   if set -q $RDF_BENCHMARKS_DATA_DIR
      if not test -d $RDF_BENCHMARKS_DATA_DIR
         echo "Environment variable RDF_BENCHMARKS_DATA_DIR is set to $RDF_BENCHMARKS_DATA_DIR, but that is not a directory. Aborting" 1>&2
         exit 1
      end
      echo $RDF_BENCHMARKS_DATA_DIR
      return
   end

   # otherwise no RDF_BENCHMARKS_DATA_DIR
   set msg "Environment variable RDF_BENCHMARKS_DATA_DIR not set,"
   if test -d "$PWD/data"
      echo $msg "using $PWD/data instead." 1>&2
      echo "$PWD/data"
   else
      echo $msg "fallback $PWD/data does not exist, aborting." 1>&2
      exit 1
   end
end

# usage: run_bench benchmark_name [extra_args...]
# will call ./$WORKDIR/benchmark_name> n_threads [extra_args...]
function run_bench
   argparse -N1 -- $argv
   set exe "$WORKDIR/$argv[1]"
   if not test -x "$exe"
      echo "Could not find $exe!" 1>&2
      return 1
   end

   set logfile (mktemp)

   echo
   echo "#$argv[1]"
   echo "#cores, wall, CPU, jit"
   for n_threads in $THREAD_COUNTS
      echo -n "$n_threads, "
      $exe $n_threads $argv[2..] 2> "$logfile" # RDF logs are on stderr
      if test $status -ne 0
         echo "There was a problem running $exe! Logs are at $logfile" 1>&2
         return 1
      end
      parse_rdf_logs $logfile
   end
end

####### MAIN ######
set WORKDIR (status dirname)
set DATA_DIR (get_data_dir)
run_bench without_io
run_bench dimuonanalysis $DATA_DIR/data/Run2012BC_DoubleMuParked_Muons.zstd.root
