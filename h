DynamicCumulativeMargin =
VAR CurrentPercentile = MAX('PercentileAxis'[Percentage of Merchants (%)])

-- Add GroupValue and CustomerMargin per customer
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

-- Rank customers *within their group*
VAR RankedTable =
    ADDCOLUMNS(
        GroupedTable,
        "CustomerPercentile",
            VAR ThisGroup = [GroupValue]
            VAR GroupFiltered =
                FILTER(GroupedTable, [GroupValue] = ThisGroup)
            VAR ThisCustomer = [CustomerMargin]
            RETURN
                ROUND(
                    DIVIDE(
                        RANKX(
                            GroupFiltered,
                            [CustomerMargin],
                            ,
                            DESC,
                            Dense
                        ),
                        COUNTROWS(GroupFiltered)
                    ) * 100,
                    0
                )
    )

-- Calculate cumulative value up to selected percentile
VAR CumulativeMargin =
    SUMX(
        FILTER(RankedTable, [CustomerPercentile] <= CurrentPercentile),
        [CustomerMargin]
    )

RETURN
    CumulativeMargin
