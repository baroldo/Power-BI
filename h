DynamicCumulativeMargin =
VAR CurrentPercentile = MAX('PercentileAxis'[Percentage of Merchants (%)])

-- Dynamically detect the active legend field
VAR GroupedTable =
    ADDCOLUMNS(
        ALLSELECTED('MARGIN_DECILIES'),
        "GroupValue",
            SWITCH(
                TRUE(),
                ISINSCOPE('MARGIN_DECILIES'[INDUSTRY]),     'MARGIN_DECILIES'[INDUSTRY],
                ISINSCOPE('MARGIN_DECILIES'[PROD_CAT]),     'MARGIN_DECILIES'[PROD_CAT],
                ISINSCOPE('MARGIN_DECILIES'[SECTOR_DESC]),  'MARGIN_DECILIES'[SECTOR_DESC],
                "All"
            ),
        "CustomerMargin", CALCULATE(SUM('MARGIN_DECILIES'[CustomerPercentileRankIncremental]))
    )

-- Rank customers within their group
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

-- Cumulative total for customers up to current percentile in their group
VAR CumulativeMargin =
    SUMX(
        FILTER(RankedTable, [CustomerPercentile] <= CurrentPercentile),
        [CustomerMargin]
    )

RETURN
    CumulativeMargin
