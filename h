DynamicCumulativeMarginPercent = 
VAR CurrentPercentile = MAX('PercentileAxis'[Percentage of Merchants (%)])

-- Step 1: Get selected group field from disconnected table
VAR SelectedGroupField = SELECTEDVALUE('Segment Table'[GroupField])

-- Step 2: Build virtual table with dynamic grouping
VAR BaseTable =
    ADDCOLUMNS(
        ALLSELECTED(MARGIN_DECILIES),
        "GroupValue",
        SWITCH(
            SelectedGroupField,
            "Category", MARGIN_DECILIES[Category],
            "Region",   MARGIN_DECILIES[Region],
            "Segment",  MARGIN_DECILIES[Segment]
        ),
        "Margin", MARGIN_DECILIES[CustomerPercentileRankIncremental]
    )

-- Step 3: Rank within group
VAR RankedTable =
    ADDCOLUMNS(
        BaseTable,
        "CustomerPercentile",
        VAR ThisGroup = [GroupValue]
        RETURN
            ROUND(
                DIVIDE(
                    RANKX(
                        FILTER(BaseTable, [GroupValue] = ThisGroup),
                        [Margin],
                        ,
                        DESC,
                        DENSE
                    ),
                    COUNTROWS(FILTER(BaseTable, [GroupValue] = ThisGroup))
                ) * 100,
                0
            )
    )

-- Step 4: Calculate cumulative percentage
VAR CumulativeMargin =
    SUMX(
        FILTER(RankedTable, [CustomerPercentile] <= CurrentPercentile),
        [Margin]
    )

VAR TotalMargin =
    SUMX(RankedTable, [Margin])

RETURN
    DIVIDE(CumulativeMargin, TotalMargin)
