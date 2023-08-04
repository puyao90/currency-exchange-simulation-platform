#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
// #include <stdexcept>
#include <sstream>
#include "Candlestick.h"
#include "Chart.h"


TextMonitor::TextMonitor(int h, int w) : height(h), width(w), screen(h, std::string(w, ' ')) {}

void TextMonitor::draw() const {
        for (const auto &row: screen) {
                std::cout << row << std::endl;
        }
    }

void TextMonitor::set(int x, int y, char ch) {
        if (x >= 0 && x < height && y >= 0 && y < width) {
                screen[x][y] = ch;
            }
    }


std::string formatDouble(double value, int precision) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}


BaseChart::BaseChart(double minY, double maxY, int chartHeight, const std::vector<std::string>& xLabels,
                     int chartWidth, int xLabelWidth, int yStartOffset, const std::string& xName,
                     const std::string& yName)
    : minY(minY), maxY(maxY), chartHeight(chartHeight), xLabels(xLabels), chartWidth(chartWidth),
      xLabelWidth(xLabelWidth), yStartOffset(yStartOffset), monitor(chartHeight + 2, chartWidth + 2),
      xName(xName), yName(yName) {}

void BaseChart::drawXAxis() {
    int y = chartHeight;
    for (int i = yStartOffset; i < chartWidth; ++i) {
        if (i == yStartOffset) {
            monitor.set(y, i, '+');
        } else {
            monitor.set(y, i, '-');
        }
    }
    for (const auto& label : xLabels) {
        std::string labelStr = label;
        printStringCentered(getXCoordinate(label), chartHeight + 1, labelStr);
    }
}

void BaseChart::drawYAxis() {
    int x = yStartOffset;
    double interval = (maxY - minY) / (chartHeight - 1);
    int labelCount = chartHeight - 1;
    for (int y = 0; y < chartHeight; ++y) {
        monitor.set(y, x, '|');
        if (y == 0) {
            continue;
        }
        double label = minY + (labelCount - y) * interval;
        std::string labelStr = formatDouble(label, 8);
        int labelLength = labelStr.length();
        int startX = x - labelLength - 1;
        printString(startX, y, labelStr);
    }
}

int BaseChart::getYCoordinate(double yValue) {
    double xRatio = (yValue - minY) / (maxY - minY);
    
    int y = static_cast<int>(1+xRatio * (chartHeight));
    int realY = flipYCoordinate(y, chartHeight);
    return realY;
}

int BaseChart::flipYCoordinate(int y, int height) {
    int midLine = height / 2;
    int flippedY = midLine - (y - midLine);
    return flippedY;
}

int BaseChart::getXCoordinate(const std::string& xValue) {
    int labelSize = xLabels.size();
    int index = 0;
    for (int i = 0; i < labelSize; ++i) {
        if (xLabels[i] == xValue) {
            index = i;
            break;
        }
    }
    int ret = (index * chartWidth) / (labelSize) + 1.5 * yStartOffset;
    return ret;
}

void BaseChart::printString(int x, int y, const std::string& str) {
    int strLength = str.length();
    for (int i = 0; i < strLength; ++i) {
        monitor.set(y, x + i, str[i]);
    }
}

void BaseChart::printStringCentered(int x, int y, const std::string& str) {
    int strLength = str.length();
    int startX = x - strLength / 2;
    for (int i = 0; i < strLength; ++i) {
        monitor.set(y, startX + i, str[i]);
    }
}


CandlestickChart::CandlestickChart(double minY, double maxY, int chartHeight, 
                const std::vector<std::string>& xLabels, int chartWidth, int xLabelWidth, 
                int yStartOffset, const std::string& xName, const std::string& yName)
    : BaseChart(minY, maxY, chartHeight, xLabels, chartWidth, xLabelWidth, 
    yStartOffset, xName, yName) {}

void CandlestickChart::draw(const std::vector<Candlestick>& candlesticks) {
    xLabels.push_back("");
    drawXAxis();
    drawYAxis();
    drawData(candlesticks);
    printString(2, chartHeight + 1, xName);
    printString(2, 0, yName);
    monitor.draw();
}

void CandlestickChart::drawLineBetweenY(double y1, double y2, int x) {
    int startY = std::min(y1, y2);
    int endY = std::max(y1, y2);

    for (int y = startY; y <= endY; ++y) {
        monitor.set(y, x, '|');
    }
}

void CandlestickChart::drawData(const std::vector<Candlestick>& candlesticks) {
    for (const auto& cs : candlesticks) {
        int x = getXCoordinate(cs.timestamp);
        double yOpen = getYCoordinate(cs.open);
        double yClose = getYCoordinate(cs.close);
        double yHigh = getYCoordinate(cs.high);
        double yLow = getYCoordinate(cs.low);

        if (yOpen < yClose) {
            drawLineBetweenY(yOpen, yHigh, x);
            drawLineBetweenY(yLow, yClose, x);
        } else {
            drawLineBetweenY(yClose, yHigh, x);
            drawLineBetweenY(yLow, yOpen, x);
        }

        for (int i = x - 2; i <= x + 2; ++i) {
            monitor.set(yClose, i, '-');
        }

        for (int i = x - 2; i <= x + 2; ++i) {
            monitor.set(yOpen, i, '-');
        }
    }
}


void generateCandlestickChart(std::vector<Candlestick> candlesticks) {
    double minY = 9999;
    double maxY = -1;
    int chartHeight = 30;
    for (const auto& cs : candlesticks) {
        minY = std::min(minY, cs.low);
        maxY = std::max(maxY, cs.high);
    }
    // std::cout << "maxY: " << maxY << std::endl;
    // std::cout << "minY: " << minY << std::endl;
    std::vector<std::string> xLabels;
 
    for (const auto& cs : candlesticks) {
        xLabels.push_back(cs.timestamp);
    }
    int chartWidth = xLabels.size() * 20;

    CandlestickChart chart(minY, maxY, chartHeight, xLabels, 
    chartWidth, 20, 16, "Date", "Price");
    chart.draw(candlesticks);
}

ScatterChart::ScatterChart(double minY, double maxY, int chartHeight, 
    const std::vector<std::string> &xLabels, int chartWidth, int xLabelWidth, 
    int yStartOffset, const std::string& xName, const std::string& yName)
    : BaseChart(minY, maxY, chartHeight, xLabels, chartWidth, xLabelWidth, 
    yStartOffset, xName, yName) {}

void ScatterChart::draw(const std::vector<OrderBookEntry> &data, double minX, double maxX) {
    xLabels.push_back("");
    drawXAxis();
    drawYAxis();
    printString(2, chartHeight + 1, xName);
    printString(2, 0, yName);
    setMinX(minX);
    setMaxX(maxX);
    drawData(data);
    monitor.draw();
}

void ScatterChart::setMinX(double value) {
    minX = value;
}

void ScatterChart::setMaxX(double value) {
    maxX = value;
}

void ScatterChart::drawData(const std::vector<OrderBookEntry> &data) {
    for (const auto &exchange: data) {
        int x = getXCoordinate2(exchange.amount);
        int y = getYCoordinate(exchange.price);
        monitor.set(y, x, 'X');
    }
}

int ScatterChart::getXCoordinate2(double xValue) {
    double xRatio = (xValue - minX) / (maxX - minX);
    int x2 = static_cast<int>(xRatio * (chartWidth));
    return yStartOffset * 1.5 + x2;
}


std::vector<std::string> generatePoints(double minAmount, double maxAmount) {
    std::vector<std::string> points;
    double interval = (maxAmount - minAmount) / 6.0;  // 6 intervals to get 7 points
    double value = minAmount;

    for (int i = 0; i < 7; ++i) {
        std::stringstream stream;
        // Format value with 4 decimal places
        stream << std::fixed << std::setprecision(4) << value;  
        points.push_back(stream.str());
        value += interval;
    }

    return points;
}


void generateScatterChart(std::vector<OrderBookEntry> exchangeData) {
    double minPrice = std::numeric_limits<double>::max();
    double maxPrice = std::numeric_limits<double>::lowest();
    double minAmount = std::numeric_limits<double>::max();
    double maxAmount = std::numeric_limits<double>::lowest();

    for (const auto& data : exchangeData) {
        minPrice = std::min(minPrice, data.price);
        maxPrice = std::max(maxPrice, data.price);
        minAmount = std::min(minAmount, data.amount);
        maxAmount = std::max(maxAmount, data.amount);
    }

    int chartHeight = 30;

    std::vector<std::string> xLabels = generatePoints(minAmount, maxAmount);

    ScatterChart scatterChart(minPrice, maxPrice, chartHeight, 
        xLabels, 100, 20, 14, " Amount ", "Price");
    scatterChart.draw(exchangeData, minAmount, maxAmount );
}