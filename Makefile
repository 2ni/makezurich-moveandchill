.PHONY: compile st serial terminals terminal

compile:
	@pio run

clean:
	@pio run -t clean

upload:
	@pio run -t upload && ./wait.py -u

serial:
	@./wait.py
