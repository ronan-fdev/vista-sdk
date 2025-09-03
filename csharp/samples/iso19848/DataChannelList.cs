using Vista.SDK;
using Vista.SDK.Transport;
using Vista.SDK.Transport.DataChannel;

static class DataChannelList
{
    // You should define the properties for each DataChannel, not default like this
    private static readonly Property DefaultProperty =
        new()
        {
            DataChannelType = new DataChannelType
            {
                Type = "Inst", // See Table 1 in ISO19848 for acceptable values
                UpdateCycle = null, // Represents the cycle of updating measurement value
                CalculationPeriod = null // When a value is a result of a calculation that uses a measurement value of specific time periods, the Calculation Period shall be used to describe the said period
            },
            Format = new Format
            {
                Type = "String", // See Table 2 in ISO19848 for acceptable values
                Restriction = new Restriction { } // Restrictive properties can be defined here, such as a list of acceptable values, strict max/min value, pattern, etc
            },
            Range = null, // Mandatory for Decimal format types
            Unit = null, // Mandatory for Decimal format types
            QualityCoding = null, // If quality is included in the timeseries packages, this should be defined
            AlertPriority = null, // Madatory for Alert DataChannelType
        };

    public static DataChannelListPackage Create(string shipId, Dictionary<string, LocalId> dataChannels)
    {
        var now = DateTimeOffset.UtcNow;
        var header = new Header
        {
            Author = "DNV",
            DateCreated = now,
            ShipId = ShipId.Parse(shipId), // Either IMO number or proprietary ID. Recommend using IMO
            DataChannelListId = new ConfigurationReference
            {
                // Important reference - will be used in TimeSeriesDataPackage
                Id = "ExampleDataChannelList",
                Version = "1.0.0",
                TimeStamp = now,
            },
        };

        var dataChannelList = new Vista.SDK.Transport.DataChannel.DataChannelList(

            [
                .. dataChannels
                .Select(
                    (kvp) =>
                        new DataChannel
                        {
                            DataChannelId = new Vista.SDK.Transport.DataChannel.DataChannelId
                            {
                                ShortId = kvp.Key,
                                LocalId = kvp.Value
                            },
                            Property = DefaultProperty with {
                                // Customize properties here
                                Name = $"TC-{kvp.Key}",
                            }
                        }
                )
            ]
        );

        var package = new DataChannelListPackage
        {
            Package = new Package { Header = header, DataChannelList = dataChannelList }
        };

        return package;
    }
}
