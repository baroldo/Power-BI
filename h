DynamicCumulativeMargin =
VAR CurrentPercentile = MAX('PercentileAxis'[Value])

VAR GroupedTable =
    ADDCOLUMNS(
        ALLSELECTED('MARGIN_DECILES'),
        "CustomerMargin", CALCULATE(SUM('MARGIN_DECILES'[MARGIN_$]))
    )

VAR RankedTable =
    ADDCOLUMNS(
        GroupedTable,
        "CustomerPercentile",
            ROUND(
                DIVIDE(
                    RANKX(
                        GroupedTable,
                        [CustomerMargin],
                        ,
                        DESC,
                        Dense
                    ),
                    COUNTROWS(GroupedTable)
                ) * 100,
                0
            )
    )

VAR CumulativeMargin =
    SUMX(
        FILTER(RankedTable, [CustomerPercentile] <= CurrentPercentile),
        [CustomerMargin]
    )

RETURN
    CumulativeMargin
