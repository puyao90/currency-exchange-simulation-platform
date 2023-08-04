# include "Candlestick.h"

Candlestick::Candlestick(std::string _timestamp,
                        double _open, 
                        double _close,
                        double _high,
                        double _low)
:timestamp(_timestamp),
open(_open),
close(_close),
high(_high),low(_low){}