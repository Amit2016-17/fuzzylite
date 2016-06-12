/*
 Copyright © 2010-2016 by FuzzyLite Limited.
 All rights reserved.

 This file is part of fuzzylite®.

 fuzzylite® is free software: you can redistribute it and/or modify it under
 the terms of the FuzzyLite License included with the software.

 You should have received a copy of the FuzzyLite License along with
 fuzzylite®. If not, see <http://www.fuzzylite.com/license/>.

 fuzzylite® is a registered trademark of FuzzyLite Limited.
 */

#ifndef FL_BENCHMARK_H
#define FL_BENCHMARK_H

#include "fl/fuzzylite.h"

#include "fl/imex/FldExporter.h"
#include <string>
#include <vector>

namespace fl {

    class Engine;

    class FL_API Benchmark {
    private:
        std::string _name;
        Engine* _engine;
        std::vector<std::vector<scalar> > _expected;
        std::vector<std::vector<scalar> > _obtained;
        std::vector<long> _times;
        scalar _tolerance;

    public:

        /**
         Unit of time to utilize in the results
         */
        enum TimeUnit {
            NanoSeconds, MicroSeconds, MilliSeconds, Seconds, Minutes, Hours
        };

        /**
         Shape of the table of results
         */
        enum TableShape {
            Horizontal, Vertical
        };

        /**
         Contents of the table of results
         */
        enum TableContents {
            Header = 1, Body = 2, HeaderAndBody = (Header | Body)
        };

        /**
         Type of error between expected and obtained values
         */
        enum ErrorType {
            NonFinite, Accuracy, All
        };

        explicit Benchmark(const std::string& name = "", Engine* engine = fl::null,
                scalar tolerance = fuzzylite::macheps());
        virtual ~Benchmark();
        FL_DEFAULT_COPY_AND_MOVE(Benchmark)

        /**
         Sets the name of the benchmark
         @param name is the name of the benchmark
         */
        void setName(const std::string& name);
        /**
         Gets the name of the benchmark
         @return name is the name of the benchmark
         */
        std::string getName() const;

        /**
         Sets the engine to benchmark
         @param engine is the engine to benchmark
         */
        void setEngine(Engine* engine);
        /**
         Gets the engine to benchmark
         @return the engine to benchmark
         */
        Engine* getEngine() const;

        /**
         Sets the set of expected values from the engine, where the inner vector
         contains the input values and output values
         @param expected is the set of expected values from the engine
         */
        void setExpected(const std::vector<std::vector<scalar> >& expected);
        /**
         Gets the set of expected values from the engine, where the inner vector
         contains the input values and output values
         @return the set of expected values from the engine
         */
        const std::vector<std::vector<scalar> >& getExpected() const;

        /**
         Sets the set of obtained values from the engine, where the inner vector
         contains the input values and output values
         @param obtained is the set of obtained values from the engine
         */
        void setObtained(const std::vector<std::vector<scalar> >& obtained);
        /**
         Gets the set of obtained values from the engine, where the inner vector
         contains the input values and output values
         @return the set of obtained values from the engine
         */
        const std::vector<std::vector<scalar> >& getObtained() const;

        /**
         Sets the vector of nanoseconds taken to produce the set of obtained values
         from the set of expected input values
         @param times is the vector of nanoseconds taken to produce the set of obtained values
         from the set of expected input values
         */
        void setTimes(const std::vector<long> times);
        /**
         Gets the vector of nanoseconds taken to produce the set of obtained values
         from the set of expected input values
         @return the vector of nanoseconds taken to produce the set of obtained values
         from the set of expected input values
         */
        const std::vector<long>& getTimes() const;

        /**
         Sets the tolerance above which the difference between an expected and 
         obtained value from the engine is considered an error
         @param tolerance is the tolerance above which the difference between 
         an expected and obtained value from the engine is considered an error
         */
        void setTolerance(scalar tolerance);
        /**
         Gets the tolerance above which the difference between an expected and 
         obtained value from the engine is considered an error
         @return the tolerance above which the difference between an expected 
         and obtained value from the engine is considered an error
         */
        scalar getTolerance() const;

        /**
         Produces and loads into memory the set of expected values from the 
         engine
         @param values is the number of values to evaluate the engine upon
         @param scope is the scope of the values to generate
         */
        virtual void prepare(int values, FldExporter::ScopeOfValues scope);
        /**
         Reads and loads into memory the set of expected values from the 
         engine
         @param reader is the reader of a set of lines containing 
         space-separated  values
         */
        virtual void prepare(std::istream& reader);

        /**
         Runs the benchmark on the engine only once
         @return the time in nanoseconds required by the run, which is 
         also appended to the times stored in Benchmark::getTimes()
         */
        virtual long runOnce();
        /**
         Runs the benchmark on the engine multiple times
         @param times is the number of times to run the benchmark on the engine
         @return vector of the time in nanoseconds required by each run, which is 
         also appended to the times stored in Benchmark::getTimes()
         */
        virtual std::vector<long> run(int times);

        /**
         Resets the benchmark to be ready to run again
         */
        virtual void reset();

        /**
         Indicates whether errors can be computed based on the expected and 
         obtained values from the benchmark. If the benchmark was prepared from
         a file reader and the file included columns of expected output values
         and the benchmark has been run at least once, then the benchmark can 
         automatically compute the errors and will automatically include them in 
         the results.
         @return whether errors can be computed based on the expected and 
         obtained values from the benchmark
         */
        virtual bool canComputeErrors() const;

        /**
         Computes the mean squared error over all output variables considering 
         only those cases where there is an accuracy error as defined in 
         Benchmark::accuracyErrors().
         @return  the mean squared error over all the output variables.
         */
        virtual double meanSquaredError() const;

        /**
         Computes the number of errors over all the output variables caused by 
         non-finite differences or accuracy differences. An error is counted when 
         the difference between the expected and obtained values is not finite, 
         or the absolute difference between the expected and obtained values 
         is not smaller than the tolerance.
         @return the number of errors over all the output variables caused by 
         non-finite differences or accuracy differences
         */
        virtual int allErrors() const;

        /**
         Computes the number of errors over all the output variables caused by 
         non-finite differences (ie, infinity and NaN). An error is counted when 
         the difference between the expected and obtained values is not finite.
         @return the number of errors over all the output variables caused by 
         non-finite differences 
         */
        virtual int nonFiniteErrors() const;
        /**
         Computes the number of errors over all the output variables caused by 
         a significant difference in accuracy. An error is counted when the 
         absolute difference between the expected and  obtained values 
         is not smaller than the tolerance.
         
         @f$\text{E} = \sum_y \sum_i \epsilon_i^y, \text{where } \epsilon_i^y = 
         \begin{cases}
         0 & \text{if} |e_i^y - o^y_i| < \theta\\
         1 & \text{otherwise}
         \end{cases}
         @f$,
         @f$y@f$ is the set of output variables, @f$e@f$ is the set of 
         expected output values, @f$o@f$ is the set of obtained output values
         
         @return the number of errors over all the output variables caused by 
         a significant difference in accuracy
         */
        virtual int accuracyErrors() const;

        /**
         Computes the number of errors of the given type.
         @return the number of errors over all the output variables
         */
        virtual int numberOfErrors(ErrorType errorType) const;

        /**
         Returns the name of the time unit
         @return the name of the time unit
         */
        static std::string stringOf(TimeUnit unit);

        /**
         Returns the factor of the given unit from NanoSeconds
         @param unit is the unit of time
         @return the factor of the given unit from NanoSeconds
         */
        static scalar factorOf(TimeUnit unit);
        /**
         Converts the time to different scales
         @param time is the time to convert
         @param from is the units of the time to convert from
         @param to is the units of the time to convert to
         @return the time in the units specified
         */
        static scalar convert(scalar time, TimeUnit from, TimeUnit to);

        typedef std::pair<std::string, std::string> Result;
        /**
         Computes and returns the results from the benchmark
         @param timeUnit is the unit of time of the results
         @param includeTimes indicates whether to include the times of each run
         @return the results from the benchmark
         */
        virtual std::vector<Result> results(TimeUnit timeUnit = NanoSeconds, bool includeTimes = true) const;

        /**
         Formats the results
         @param results is the vector of results
         @param shape is the shape to present the table of results
         @param contents indicates the information to include in the table of results
         @param delimiter is the delimiter of the table of results
         */
        virtual std::string format(std::vector<Result> results, TableShape shape,
                TableContents contents, const std::string& delimiter = "\t") const;
    };

}

#endif /* FL_BENCHMARK_H */

