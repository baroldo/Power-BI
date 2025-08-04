DynamicCumulativeMargin = 
VAR SelectedGroup = SELECTEDVALUE('GroupBySelector'[GroupBySelector])

-- Defensive fallback if no selection
VAR _Check = IF(ISBLANK(SelectedGroup), BLANK())

-- Step 1: Get current percentile from axis
VAR CurrentPercentile = MAX('MARGIN_DECILIES'[Percentage of Merchants (%)])

-- Step 2: Build base table with GroupValue and CustomerMargin
VAR GroupedTable =
    ADDCOLUMNS(
        ALLSELECTED('MARGIN_DECILIES'),
        "GroupValue",
            SWITCH(
                TRUE(),
                SelectedGroup = "INDUSTRY", 'MARGIN_DECILIES'[INDUSTRY],
                SelectedGroup = "PROD_CAT", 'MARGIN_DECILIES'[PROD_CAT],
                SelectedGroup = "SECTOR_DESC", 'MARGIN_DECILIES'[SECTOR_DESC],
                "All"  // Use constant for no grouping
            ),
        "CustomerMargin", 
            CALCULATE(SUM('MARGIN_DECILIES'[CustomerPercentileRankIncremental]))
    )

-- Step 3: Rank within group
VAR RankedTable =
    ADDCOLUMNS(
        GroupedTable,
        "CustomerPercentile",
            VAR ThisGroup = [GroupValue]
            VAR ThisMargin = [CustomerMargin]
            RETURN
                ROUND(
                    DIVIDE(
                        RANKX(
                            FILTER(GroupedTable, [GroupValue] = ThisGroup),
                            [CustomerMargin],
                            ,
                            DESC,
                            DENSE
                        ),
                        COUNTROWS(FILTER(GroupedTable, [GroupValue] = ThisGroup))
                    ) * 100,
                    0
                )
    )

-- Step 4: Cumulative sum within group up to current percentile
VAR Result =
    CALCULATE(
        SUMX(
            FILTER(RankedTable, [CustomerPercentile] <= CurrentPercentile),
            [CustomerMargin]
        )
    )

RETURN
    IF(ISBLANK(_Check), BLANK(), Result)
