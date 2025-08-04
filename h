DynamicCumulativeMarginPercent = 
VAR SelectedGroup =
    SELECTEDVALUE('Segment Table'[GroupBySelector])  -- E.g., "Region"

VAR CurrentPercentile =
    MAX('PercentileAxis'[Percentage of Merchants (%)])

-- Step 1: Create base table with dynamic group values
VAR BaseTable =
    ADDCOLUMNS(
        ALLSELECTED(MARGIN_DECILIES),
        "GroupValue",
        SWITCH(
            SelectedGroup,
            "Region",    MARGIN_DECILIES[Region],
            "Category",  MARGIN_DECILIES[Category],
            "Segment",   MARGIN_DECILIES[Segment],
            "Default"
        ),
        "Margin", MARGIN_DECILIES[CustomerPercentileRankIncremental]
    )

-- Step 2: Rank within each group
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

-- Step 3: Get current group in visual context (e.g. VIC, NSW)
VAR VisualGroup = SELECTEDVALUE(MARGIN_DECILIES[Region]) -- this field will update depending on selected legend field

-- Step 4: Compute cumulative for each line in visual
VAR CumulativeMargin =
    SUMX(
        FILTER(RankedTable,
            [GroupValue] = VisualGroup &&
            [CustomerPercentile] <= CurrentPercentile
        ),
        [Margin]
    )

VAR TotalMargin =
    SUMX(
        FILTER(RankedTable, [GroupValue] = VisualGroup),
        [Margin]
    )

RETURN
    DIVIDE(CumulativeMargin, TotalMargin)
