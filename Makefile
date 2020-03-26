LIBIEC_HOME=../libiec61850

PROJECT_BINARY_NAME = server_example_inputs
PROJECT_SOURCES = server_example_inputs.c
PROJECT_SOURCES += static_model.c
PROJECT_SOURCES += static_input.c
PROJECT_SOURCES += config_file_parser_input.c
PROJECT_SOURCES += dynamic_model_input.c
PROJECT_SOURCES += inputs.c

PROJECT_ICD_FILE = simpleIO_inputs.cid

include $(LIBIEC_HOME)/make/target_system.mk
include $(LIBIEC_HOME)/make/stack_includes.mk

all:	$(PROJECT_BINARY_NAME)

include $(LIBIEC_HOME)/make/common_targets.mk

model:	$(PROJECT_ICD_FILE)
	java -jar $(LIBIEC_HOME)/tools/model_generator/genmodel.jar $(PROJECT_ICD_FILE)
	java -jar model_input_generator/genmodel_input.jar $(PROJECT_ICD_FILE)

$(PROJECT_BINARY_NAME):	$(PROJECT_SOURCES) $(LIB_NAME)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PROJECT_BINARY_NAME) $(PROJECT_SOURCES) $(INCLUDES) $(LIB_NAME) $(LDLIBS)

clean:
	rm -f $(PROJECT_BINARY_NAME)


