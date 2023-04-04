#!/usr/bin/fish

set WORKDIR (status dirname)

function parse_rdf_logs
   argparse -X1 -- $argv
   echo $argv[1] | awk '
   /Nothing to jit/ { jit_time = 0 }
   /Finished event loop number 0/ {
      match($0, /([[:digit:]]+\.[[:digit:]]+)s CPU, ([[:digit:]]+\.[[:digit:]]+)s elapsed/, arr)
      cpu_time = arr[1]
      wall_time = arr[2]
   }
   END { printf "%s, %s, %s\n", wall_time, cpu_time, jit_time  }'
end

function run_without_io
   set exe "$WORKDIR/without_io"
   if not test -x "$exe"
      echo "Could not find without_io!"
      return 1
   end

   echo "#without_io"
   echo "#cores, wall, CPU, jit"
   for n_threads in 1 4
      echo -n "$n_threads, "
      set output ($exe $n_threads 2>&1)
      parse_rdf_logs "$output"
   end
end

run_without_io
