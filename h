DynamicCumulativeMarginPercent = 
VAR SelectedGroup =
    SELECTEDVALUE('Segment Table'[GroupBySelector])  -- e.g., "Region"

VAR CurrentPercentile =
    MAX('PercentileAxis'[Percentage of Merchants (%)])

-- Step 1: Build virtual table with dynamic group column
VAR BaseTable =
    ADDCOLUMNS(
        SUMMARIZE(
            MARGIN_DECILIES,
            MARGIN_DECILIES[CustomerNumber],  -- or whatever uniquely identifies rows
            SWITCH(
                SelectedGroup,
                "Region",   MARGIN_DECILIES[Region],
                "Category", MARGIN_DECILIES[Category],
                "Segment",  MARGIN_DECILIES[Segment]
            )
        ),
        "GroupValue", 
            SWITCH(
                SelectedGroup,
                "Region",   MARGIN_DECILIES[Region],
                "Category", MARGIN_DECILIES[Category],
                "Segment",  MARGIN_DECILIES[Segment]
            ),
        "Margin", MARGIN_DECILIES[CustomerPercentileRankIncremental]
    )

-- Step 2: Rank customers within their group
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

-- Step 3: Cumulative margin within group up to current percentile
VAR CumulativeMargin =
    SUMX(
        FILTER(RankedTable,
            [CustomerPercentile] <= CurrentPercentile
        ),
        [Margin]
    )

VAR TotalMargin =
    SUMX(RankedTable, [Margin])

RETURN
    DIVIDE(CumulativeMargin, TotalMargin)
