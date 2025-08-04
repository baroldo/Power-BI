DynamicCumulativeMargin = 
VAR CurrentPercentile = MAX('PercentileAxis'[Value])

-- The legend grouping field (Region, Product, etc.)
VAR LegendField = SELECTEDVALUE('Legend Field Parameter'[Legend Field Parameter])

-- Create a table with customer, their margin, and their group value (based on selected legend field)
VAR BaseTable =
    ADDCOLUMNS(
        ALLSELECTED('MARGIN_DECILES'),
        "GroupValue", 
            SWITCH(
                TRUE(),
                LegendField = "Region", 'MARGIN_DECILES'[Region],
                LegendField = "Product", 'MARGIN_DECILES'[Product],
                LegendField = "Salesperson", 'MARGIN_DECILES'[Salesperson],
                LegendField = "CustomerGroup", 'MARGIN_DECILES'[CustomerGroup],
                "All"
            ),
        "CustomerMargin", CALCULATE(SUM('MARGIN_DECILES'[MARGIN_$]))
    )

-- Rank each customer within their own group by margin
VAR RankedTable =
    ADDCOLUMNS(
        BaseTable,
        "CustomerPercentile",
            ROUND(
                DIVIDE(
                    RANKX(
                        FILTER(BaseTable, [GroupValue] = EARLIER([GroupValue])),
                        [CustomerMargin],
                        ,
                        DESC,
                        Dense
                    ),
                    COUNTROWS(FILTER(BaseTable, [GroupValue] = EARLIER([GroupValue])))
                ) * 100,
                0
            )
    )

-- Cumulative margin for customers within group and percentile threshold
VAR CumulativeMargin =
    SUMX(
        FILTER(
            RankedTable,
            [CustomerPercentile] <= CurrentPercentile
        ),
        [CustomerMargin]
    )

RETURN
    CumulativeMargin
