DynamicCumulativeMargin =
VAR CurrentPercentile = MAX('PercentileAxis'[Value])

-- Dynamically detect the active legend field
VAR GroupedTable =
    ADDCOLUMNS(
        ALLSELECTED('MARGIN_DECILES'),
        "GroupValue",
            SWITCH(
                TRUE(),
                ISINSCOPE('MARGIN_DECILES'[Region]),        'MARGIN_DECILES'[Region],
                ISINSCOPE('MARGIN_DECILES'[Product]),       'MARGIN_DECILES'[Product],
                ISINSCOPE('MARGIN_DECILES'[Salesperson]),   'MARGIN_DECILES'[Salesperson],
                ISINSCOPE('MARGIN_DECILES'[CustomerGroup]), 'MARGIN_DECILES'[CustomerGroup],
                "All"
            ),
        "CustomerMargin", CALCULATE(SUM('MARGIN_DECILES'[MARGIN_$]))
    )

-- Rank customers within their group (Region, Product, etc.)
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

-- Cumulative margin for customers up to current percentile in their group
VAR CumulativeMargin =
    SUMX(
        FILTER(RankedTable, [CustomerPercentile] <= CurrentPercentile),
        [CustomerMargin]
    )

RETURN
    CumulativeMargin
