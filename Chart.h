#include <vector>
#include "Candlestick.h"
#include "OrderBookEntry.h"

// Represents a text-based monitor to display the chart
class TextMonitor{
    public:
        TextMonitor(int h, int w);

        // Draw the content of the monitor on the console
        void draw() const;

        // Set a character at a specific position on the monitor
        void set(int x, int y, char ch);

    private:
        int height;
        int width;
        std::vector<std::string> screen;
};

// Format a double value as a string with a specified precision
std::string formatDouble(double value, int precision);


// Base class for creating different types of charts
class BaseChart {
    protected:
        double minY;
        double maxY;
        int chartHeight;
        std::vector<std::string> xLabels;
        int chartWidth;
        int xLabelWidth;
        int yStartOffset;
        TextMonitor monitor;
        std::string xName;
        std::string yName;

    public:
        BaseChart(double minY, double maxY, int chartHeight, 
            const std::vector<std::string> &xLabels, int chartWidth,
            int xLabelWidth,int yStartOffset, const std::string &xName, 
            const std::string &yName);
        
        // Virtual destructor to allow proper destruction of derived classes
        virtual ~BaseChart() = default;

    protected:
        // Draw the X-axis of the chart
        void drawXAxis();
        // Draw the Y-axis of the chart
        void drawYAxis();
        // Get the Y-coordinate for a given Y-value
        int getYCoordinate(double yValue);
        // Flip the Y-coordinate to ensure correct orientation
        int flipYCoordinate(int y, int height);
        // Get the X-coordinate for a given X-value
        int getXCoordinate(const std::string& xValue);
        // Print a string at the specified position
        void printString(int x, int y, const std::string& str);
        // Print a string centered at the specified position
        void printStringCentered(int x, int y, const std::string& str);
};



// Chart class specifically for drawing candlestick charts
class CandlestickChart : public BaseChart {
    public:
        CandlestickChart(double minY, double maxY, int chartHeight, 
            const std::vector<std::string>& xLabels,
            int chartWidth, int xLabelWidth, int yStartOffset, 
            const std::string& xName, const std::string& yName);

        // Draw the candlestick chart on the console
        void draw(const std::vector<Candlestick>& candlesticks);

        // Draw a line between two Y-values at a specific X-coordinate
        void drawLineBetweenY(double y1, double y2, int x);

        // Draw the candlestick data on the chart
        void drawData(const std::vector<Candlestick>& candlesticks);
};

// Generate and draw a candlestick chart based on the provided candlestick data
void generateCandlestickChart(std::vector<Candlestick> candlesticks);


// Chart class specifically for drawing scatter charts
class ScatterChart : public BaseChart {
    private:
        double minX;
        double maxX;

    public:
        ScatterChart(double minY, double maxY, int chartHeight, 
            const std::vector<std::string> &xLabels, int chartWidth,
            int xLabelWidth, int yStartOffset, const std::string& xName, 
            const std::string& yName);

        // Draw the scatter chart on the console
        void draw(const std::vector<OrderBookEntry> &data, double minX, double maxX);
        // Set the minimum X-value for the chart
        void setMinX(double value);
        // Set the maximum X-value for the chart
        void setMaxX(double value);
        // Draw the scatter plot data on the chart
        void drawData(const std::vector<OrderBookEntry> &data);
        // Get the X-coordinate for a given X-value
        int getXCoordinate2(double xValue);
};

// Generate a set of points based on the minimum and maximum amount values
std::vector<std::string> generatePoints(double minAmount, double maxAmount);

// Generate and draw a scatter chart based on the provided exchange data
void generateScatterChart(std::vector<OrderBookEntry> exchangeData);


