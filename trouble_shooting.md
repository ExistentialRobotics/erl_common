# Troubleshooting

## The program is slower in docker

If you are using docker on Windows or Mac, the program may be slower than running on native Linux.
This is because docker uses a virtual machine to run Linux, which adds some overhead. You can try
to use WSL2 on Windows or run Linux natively on your Mac to improve the performance.
With Linux docker, native performance should be achieved. If it does not, please run the program
with `OMP_DISPLAY_ENV=verbose` and check if the OpenMP environment variables are set correctly.
Here are the expected OMP environment variables:

```shell
OPENMP DISPLAY ENVIRONMENT BEGIN
  _OPENMP = '201511'
  [host] OMP_DYNAMIC = 'FALSE'
  [host] OMP_NESTED = 'FALSE'
  [host] OMP_NUM_THREADS = '32'
  [host] OMP_SCHEDULE = 'DYNAMIC'
  [host] OMP_PROC_BIND = 'FALSE'
  [host] OMP_PLACES = ''
  [host] OMP_STACKSIZE = '0'
  [host] OMP_WAIT_POLICY = 'PASSIVE'
  [host] OMP_THREAD_LIMIT = '4294967295'
  [host] OMP_MAX_ACTIVE_LEVELS = '1'
  [host] OMP_NUM_TEAMS = '0'
  [host] OMP_TEAMS_THREAD_LIMIT = '0'
  [all] OMP_CANCELLATION = 'FALSE'
  [all] OMP_DEFAULT_DEVICE = '0'
  [all] OMP_MAX_TASK_PRIORITY = '0'
  [all] OMP_DISPLAY_AFFINITY = 'FALSE'
  [host] OMP_AFFINITY_FORMAT = 'level %L thread %i affinity %A'
  [host] OMP_ALLOCATOR = 'omp_default_mem_alloc'
  [all] OMP_TARGET_OFFLOAD = 'DEFAULT'
  [host] GOMP_CPU_AFFINITY = ''
  [host] GOMP_STACKSIZE = '0'
  [host] GOMP_SPINCOUNT = '1'
OPENMP DISPLAY ENVIRONMENT END
```

Pay attention to `GOMP_SPINCOUNT`, which is default to `300000` in Ubuntu 20.04 and slows down the
program significantly. You can set it to `1` in your shell configuration file, or run the program
with `GOMP_SPINCOUNT=1` to improve the performance. e.g.,

```shell
GOMP_SPINCOUNT=1 ./your_program
```
