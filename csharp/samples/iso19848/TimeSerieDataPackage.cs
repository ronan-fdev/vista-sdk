using System.Globalization;
using Vista.SDK;
using Vista.SDK.Transport;
using Vista.SDK.Transport.TimeSeries;
using DataChannel = Vista.SDK.Transport.DataChannel;

record TurboChargerData(
    DateTimeOffset Timestamp,
    double Speed,
    double LubeOilPressureInlet,
    double LubeOilTemperatureInlet,
    double LubeOilTemperatureOutlet,
    double ExhaustGasTemperatureOutlet,
    (double Lat, double Lng) Position
);

static class TimeSeries
{
    internal static TimeSeriesDataPackage GetPackage(
        string shipId,
        DataChannel.DataChannelListPackage dataChannelListPackage,
        IReadOnlyList<TurboChargerData> sourceData
    )
    {
        // Shipboard server extracts sensor data and prepares to ship it to shore using the TimeSeriesDataPackage
        var now = DateTimeOffset.UtcNow;
        var dataChannelId = dataChannelListPackage
            .DataChannelList
            .Select(d => DataChannelId.Parse(d.DataChannelId.ShortId!))
            .ToList();

        // Order of the properties have to match the DataChannelId order
        var dataAsTable = sourceData
            .Select(
                e =>
                    (
                        e.Timestamp,
                        Data: new[]
                        {
                            e.Speed.ToString(CultureInfo.InvariantCulture),
                            e.LubeOilPressureInlet.ToString(CultureInfo.InvariantCulture),
                            e.LubeOilTemperatureInlet.ToString(CultureInfo.InvariantCulture),
                            e.LubeOilTemperatureOutlet.ToString(CultureInfo.InvariantCulture),
                            e.ExhaustGasTemperatureOutlet.ToString(CultureInfo.InvariantCulture),
                            e.Position.Lat.ToString(CultureInfo.InvariantCulture),
                            e.Position.Lng.ToString(CultureInfo.InvariantCulture),
                        }
                    )
            )
            .ToArray();

        var from = sourceData.Min(d => d.Timestamp);
        var to = sourceData.Max(d => d.Timestamp);

        var data = new TimeSeriesData
        {
            DataConfiguration = new ConfigurationReference
            {
                Id = dataChannelListPackage.Package.Header.DataChannelListId.Id,
                TimeStamp = dataChannelListPackage.Package.Header.DataChannelListId.TimeStamp
            }, // Reference to the DataChannelList used - optional, but highly recommended
            EventData = null,
            TabularData =
            [
                new TabularData
                {
                    DataChannelIds = dataChannelId,
                    DataSets = [.. dataAsTable
                        .Select(
                            d =>
                                new TabularDataSet
                                {
                                    TimeStamp = d.Timestamp,
                                    Value = [.. d.Data],
                                    Quality = null
                                }
                        )],
                }
            ]
        };

        // Optional validation step - can be used to validate data
        var result = data.Validate(
            dataChannelListPackage,
            // Tabular
            (timestamp, dataChannel, value, quality) =>
            {
                // Custom validation logic
                return new ValidateResult.Ok();
            },
            // Event
            (timestamp, dataChannel, value, quality) =>
            {
                // Custom validation logic
                return new ValidateResult.Ok();
            }
        );
        if (result is ValidateResult.Invalid invalid)
        {
            throw new InvalidOperationException($"The data is invalid: {string.Join(", ", invalid.Messages)}");
        }

        var header = new Header
        {
            Author = "DNV",
            DateCreated = now,
            DateModified = now,
            ShipId = ShipId.Parse(shipId),
            SystemConfiguration =  [], // A reference to the source system, e.g. DBMS
            TimeSpan = new Vista.SDK.Transport.TimeSeries.TimeSpan { Start = from, End = to } // Oldest and newest dataset
        };
        var package = new TimeSeriesDataPackage
        {
            Package = new Package { Header = header, TimeSeriesData =  [data] }
        };
        return package;
    }
}
