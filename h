DynamicCumulativeMargin = 
VAR CurrentPercentile = MAX('PercentileAxis'[Value])

-- Get the selected category name from disconnected slicer
VAR SelectedCategory = SELECTEDVALUE('CategorySelector'[CategoryName])

-- Rebuild table based on selected group (using the LegendLabel)
VAR GroupedTable =
    ADDCOLUMNS(
        ALLSELECTED(
            'MARGIN_DECILES'[Customer Number],
            'MARGIN_DECILES'[LegendLabel]
        ),
        "CustomerMargin", CALCULATE(SUM('MARGIN_DECILES'[MARGIN_$]))
    )

-- Add percentile ranking within group
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

-- Cumulative margin up to selected percentile
VAR CumulativeMargin =
    CALCULATE(
        SUMX(
            FILTER(RankedTable, [CustomerPercentile] <= CurrentPercentile),
            [CustomerMargin]
        )
    )

RETURN
    CumulativeMargin
