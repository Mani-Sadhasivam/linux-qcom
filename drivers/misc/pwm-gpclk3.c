#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>

struct gpclk3_pwm {
	struct device *dev;
	struct clk *clk;
};

static int gpclk3_pwm_probe(struct platform_device *pdev)
{
	struct gpclk3_pwm *data;
	int ret;

	data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->dev = &pdev->dev;

	/* get gpclk3 */
	data->clk = devm_clk_get(&pdev->dev, "gpclk3");
	if (IS_ERR(data->clk)) {
		dev_err(&pdev->dev, "gpclk3 not found: %ld\n", PTR_ERR(data->clk));
		return PTR_ERR(data->clk);
	}

	/* set clock frequency */
	ret = clk_set_rate(data->clk, 19200000);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to set clock frequency to 19.2MHz: %d\n", ret);
		return ret;
	}
	dev_info(&pdev->dev, "gpclk3 rate set to %lu\n", clk_get_rate(data->clk));

	/* enable clock */
	ret = clk_prepare_enable(data->clk);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to enable clock: %d\n", ret);
		return ret;
	}
	
	platform_set_drvdata(pdev, data);

	dev_info(&pdev->dev, "gpclk3-pwm loaded\n");	

	return 0;
}

static int gpclk3_pwm_remove(struct platform_device *pdev)
{
	struct gpclk3_pwm *data = platform_get_drvdata(pdev);

	clk_disable_unprepare(data->clk);

	return 0;
}

static const struct of_device_id gpclk3_pwm_of_match[] = {
	{ .compatible = "qcom, gpclk3-pwm" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, gpclk3_pwm_of_match);

static struct platform_driver gpclk3_pwm_driver = {
	.driver = {
		.name = "gpclk3-pwm",
		.of_match_table = gpclk3_pwm_of_match,
	},
	.probe = gpclk3_pwm_probe,
	.remove = gpclk3_pwm_remove,
};
module_platform_driver(gpclk3_pwm_driver);

MODULE_AUTHOR("Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>");
MODULE_DESCRIPTION("Qualcomm sample PWM driver based on GPCLK module");
MODULE_LICENSE("GPL");
