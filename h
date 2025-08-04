DynamicCumulativeMarginPercent = 
VAR SelectedGroup =
    SELECTEDVALUE('Segment Table'[GroupBySelector])  -- E.g. "Region"

VAR CurrentPercentile =
    MAX('PercentileAxis'[Percentage of Merchants (%)])

-- Dynamically get the column based on the selected group
VAR BaseTable =
    ADDCOLUMNS(
        ALLSELECTED(MARGIN_DECILIES),
        "GroupValue", 
            SWITCH(
                SelectedGroup,
                "Region",   MARGIN_DECILIES[Region],
                "Category", MARGIN_DECILIES[Category],
                "Segment",  MARGIN_DECILIES[Segment],
                BLANK()
            ),
        "Margin", MARGIN_DECILIES[CustomerPercentileRankIncremental]
    )

-- Calculate percentile rank within each group
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

-- Get the current group shown in the legend field of the chart
VAR CurrentLegendGroup = MAXX(VALUES(BaseTable[GroupValue]), [GroupValue])

-- Calculate cumulative and total margin per group
VAR CumulativeMargin =
    SUMX(
        FILTER(RankedTable,
            [GroupValue] = CurrentLegendGroup &&
            [CustomerPercentile] <= CurrentPercentile
        ),
        [Margin]
    )

VAR TotalMargin =
    SUMX(
        FILTER(RankedTable,
            [GroupValue] = CurrentLegendGroup
        ),
        [Margin]
    )

RETURN
    DIVIDE(CumulativeMargin, TotalMargin)
