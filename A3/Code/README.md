# TCP Congestion control algorithm simulator

### To run the simulator

`$ python3 mytcp.py -i <double> -m <double> -n <double> -f <double> -s <double> -T <int> -o outfile`

- Congestion window sizes are output in `outfile`
  - updated window size is output in a new line
  - by default, output is stored in `results.txt`
- By default, generated plot is stored as `plot.png`
  - to change output plot name, use `-a <plotName>`
- Make sure `matplotlib` package is available for importing
  - if not available install using `pip install matplotlib`

To generate all the plots mentioned in the problem statement at once, run `sh gen.sh`
