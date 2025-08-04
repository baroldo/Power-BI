DynamicCumulativeMargin = 
VAR SelectedCategory =
    SELECTEDVALUE('GroupBySelector'[GroupBySelector], "All")

VAR CurrentPercentile =
    MAX('PercentileAxis'[Percentage of Merchants (%)])

-- Step 1: Add dynamic grouping and calculate margin
VAR GroupedTable =
    ADDCOLUMNS(
        ALLSELECTED('MARGIN_DECILIES'),
        "GroupValue",
            SWITCH(
                SelectedCategory,
                "INDUSTRY", 'MARGIN_DECILIES'[INDUSTRY],
                "PROD_CAT", 'MARGIN_DECILIES'[PROD_CAT],
                "SECTOR_DESC", 'MARGIN_DECILIES'[SECTOR_DESC],
                "All", "All"
            ),
        "CustomerMargin", 'MARGIN_DECILIES'[CustomerPercentileRankIncremental]
    )

-- Step 2: Rank each row within its group
VAR RankedTable =
    ADDCOLUMNS(
        GroupedTable,
        "CustomerPercentile",
            ROUND(
                DIVIDE(
                    RANKX(
                        FILTER(GroupedTable, [GroupValue] = EARLIER([GroupValue])),
                        [CustomerMargin],
                        ,
                        DESC,
                        Dense
                    ),
                    COUNTROWS(FILTER(GroupedTable, [GroupValue] = EARLIER([GroupValue])))
                ) * 100,
                0
            )
    )

-- Step 3: Calculate cumulative sum within the percentile threshold and group
VAR CumulativeMargin =
    SUMX(
        FILTER(RankedTable,
            [CustomerPercentile] <= CurrentPercentile
        ),
        [CustomerMargin]
    )

RETURN
    CumulativeMargin
