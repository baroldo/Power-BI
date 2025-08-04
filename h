DynamicCumulativeMargin = 
VAR SelectedGroup = SELECTEDVALUE('GroupBySelector'[GroupBySelector])

-- Return blank if no selection is made
VAR _Check = IF(ISBLANK(SelectedGroup), BLANK())

-- Return the corresponding column for the selected group
VAR GroupedTable =
    ADDCOLUMNS(
        ALLSELECTED('MARGIN_DECILIES'),
        "GroupValue", 
            SWITCH(
                SelectedGroup,
                "INDUSTRY", 'MARGIN_DECILIES'[INDUSTRY],
                "PROD_CAT", 'MARGIN_DECILIES'[PROD_CAT],
                "SECTOR_DESC", 'MARGIN_DECILIES'[SECTOR_DESC],
                "All", "All"
            ),
        "CustomerMargin", 
            CALCULATE(SUM('MARGIN_DECILIES'[CustomerPercentileRankIncremental]))
    )

-- Current percentile from axis (e.g., 10%, 20%, etc.)
VAR CurrentPercentile = MAX('MARGIN_DECILIES'[Percentage of Merchants (%)])

-- Apply ranking within each group
VAR RankedTable =
    ADDCOLUMNS(
        GroupedTable,
        "CustomerPercentile",
            ROUND(
                DIVIDE(
                    RANKX(
                        FILTER(
                            GroupedTable,
                            [GroupValue] = EARLIER([GroupValue])
                        ),
                        [CustomerMargin],
                        ,
                        DESC,
                        DENSE
                    ),
                    COUNTROWS(
                        FILTER(
                            GroupedTable,
                            [GroupValue] = EARLIER([GroupValue])
                        )
                    )
                ) * 100,
                0
            )
    )

-- Sum margin for customers <= current percentile within group
VAR Result =
    CALCULATE(
        SUMX(
            FILTER(RankedTable, [CustomerPercentile] <= CurrentPercentile),
            [CustomerMargin]
        )
    )

RETURN
    IF(ISBLANK(_Check), BLANK(), Result)
