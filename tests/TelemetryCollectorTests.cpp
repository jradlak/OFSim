#include <gtest/gtest.h>

#include "../src/simulation/TelemetryCollector.h"

namespace
{
using namespace ofsim_simulation;

TelemetryData sample(u64 time, f64 speed)
{
    TelemetryData data{};
    data.time = time;
    data.velocity = speed;
    data.altitude = speed * 2.0;
    data.atmPressure = speed * 3.0;
    data.position = dvec3(static_cast<f64>(time), 0.0, 0.0);
    return data;
}

TEST(TelemetryCollectorTests, SpeedRateUsesActualIntervalsAndFirstSampleIsABaseline)
{
    TelemetryCollector collector;
    collector.registerTelemetry(sample(0, 10.0));
    collector.registerTelemetry(sample(1500, 13.0));
    collector.registerTelemetry(sample(4500, 7.0));
    ASSERT_EQ(collector.accelerationHistory.size(), 3);
    EXPECT_DOUBLE_EQ(collector.accelerationHistory[0], 0.0);
    EXPECT_DOUBLE_EQ(collector.accelerationHistory[1], 2.0);
    EXPECT_DOUBLE_EQ(collector.accelerationHistory[2], -2.0);
    EXPECT_DOUBLE_EQ(collector.maxAcceleration, 2.0);
    EXPECT_DOUBLE_EQ(collector.minAcceleration, -2.0);
    EXPECT_DOUBLE_EQ(collector.telemetryHistory.back().acceleration, -2.0);
}

TEST(TelemetryCollectorTests, PausedEarlyAndOutOfOrderSamplesDoNotChangeTheBaseline)
{
    TelemetryCollector collector;
    collector.registerTelemetry(sample(2000, 10.0));
    collector.registerTelemetry(sample(2000, 100.0));
    collector.registerTelemetry(sample(1000, 100.0));
    collector.registerTelemetry(sample(2999, 100.0));
    ASSERT_EQ(collector.velocityHistory.size(), 1);
    EXPECT_DOUBLE_EQ(collector.maxVelocity, 10.0);
    collector.registerTelemetry(sample(3000, 12.0));
    ASSERT_EQ(collector.velocityHistory.size(), 2);
    EXPECT_DOUBLE_EQ(collector.accelerationHistory.back(), 2.0);
}

TEST(TelemetryCollectorTests, PlotHistoriesStayAlignedWhenTheyReachTheirLimit)
{
    TelemetryCollector collector;
    const u64 samples = TelemetryCollector::plotHistoryCapacity + 10;
    for (u64 index = 0; index < samples; ++index)
        collector.registerTelemetry(sample(index * 1000, static_cast<f64>(index * index)));

    ASSERT_EQ(collector.velocityHistory.size(), TelemetryCollector::plotHistoryCapacity);
    ASSERT_EQ(collector.altitudeHistory.size(), collector.velocityHistory.size());
    ASSERT_EQ(collector.atmPressureHistory.size(), collector.velocityHistory.size());
    ASSERT_EQ(collector.accelerationHistory.size(), collector.velocityHistory.size());
    for (std::size_t index = 0; index < collector.velocityHistory.size(); ++index)
    {
        const f64 sampleIndex = index + 10;
        EXPECT_DOUBLE_EQ(collector.velocityHistory[index], sampleIndex * sampleIndex);
        EXPECT_DOUBLE_EQ(collector.altitudeHistory[index], sampleIndex * sampleIndex * 2.0);
        EXPECT_DOUBLE_EQ(collector.atmPressureHistory[index], sampleIndex * sampleIndex * 3.0);
        EXPECT_DOUBLE_EQ(collector.accelerationHistory[index], 2.0 * sampleIndex - 1.0);
    }
}

TEST(TelemetryCollectorTests, FlightHistoryRemainsBoundedAndSpansTheWholeFlight)
{
    TelemetryCollector collector;
    const u64 samples = TelemetryCollector::flightHistoryCapacity * 10;
    for (u64 index = 0; index < samples; ++index)
    {
        collector.registerTelemetry(sample(index * 1000, static_cast<f64>(index)));
        ASSERT_LE(collector.telemetryHistory.size(), TelemetryCollector::flightHistoryCapacity);
        ASSERT_FALSE(collector.telemetryHistory.empty());
        EXPECT_EQ(collector.telemetryHistory.front().time, 0);
        EXPECT_EQ(collector.telemetryHistory.back().time, index * 1000);
        EXPECT_EQ(collector.telemetryHistory.back().position, dvec3(index * 1000, 0.0, 0.0));
    }

    // Decimation must retain samples throughout the flight, not just its endpoints.
    const u64 finalTime = (samples - 1) * 1000;
    u64 previous = collector.telemetryHistory.front().time;
    for (std::size_t index = 1; index < collector.telemetryHistory.size(); ++index)
    {
        const auto time = collector.telemetryHistory[index].time;
        EXPECT_GT(time, previous);
        EXPECT_LE(time - previous, finalTime / (TelemetryCollector::flightHistoryCapacity / 2));
        EXPECT_DOUBLE_EQ(collector.telemetryHistory[index].velocity, time / 1000.0);
        previous = time;
    }
}

TEST(TelemetryCollectorTests, ClearRestoresSamplingExtremaAndFullHistoryResolution)
{
    TelemetryCollector collector;
    for (u64 index = 0; index < TelemetryCollector::flightHistoryCapacity * 3; ++index)
        collector.registerTelemetry(sample(index * 1000, 100.0));
    collector.clear();
    EXPECT_TRUE(collector.telemetryHistory.empty());
    EXPECT_TRUE(collector.velocityHistory.empty());
    EXPECT_TRUE(collector.altitudeHistory.empty());
    EXPECT_TRUE(collector.atmPressureHistory.empty());
    EXPECT_TRUE(collector.accelerationHistory.empty());
    EXPECT_DOUBLE_EQ(collector.maxVelocity, 0.0);
    EXPECT_DOUBLE_EQ(collector.maxAltitude, 0.0);
    EXPECT_DOUBLE_EQ(collector.maxAtmPressure, 0.0);
    EXPECT_DOUBLE_EQ(collector.maxAcceleration, 0.0);
    EXPECT_DOUBLE_EQ(collector.minAcceleration, 0.0);

    collector.registerTelemetry(sample(0, 0.0));
    collector.registerTelemetry(sample(1000, 3.0));
    collector.registerTelemetry(sample(2000, 1.0));
    ASSERT_EQ(collector.telemetryHistory.size(), 3);
    ASSERT_EQ(collector.accelerationHistory.size(), 3);
    EXPECT_DOUBLE_EQ(collector.accelerationHistory[0], 0.0);
    EXPECT_DOUBLE_EQ(collector.accelerationHistory[1], 3.0);
    EXPECT_DOUBLE_EQ(collector.accelerationHistory[2], -2.0);
}
} // namespace
