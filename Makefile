LIBIEC_HOME=../libiec61850

PROJECT_BINARY_NAME = server_example_inputs

PROJECT_ICD_FILE = simpleIO_inputs.cid

PROJECT_SOURCES = input/inputs.c
PROJECT_SOURCES += model/config_file_parser_extensions.c
PROJECT_SOURCES += model/dynamic_model_extensions.c

PROJECT_SOURCES += server_example_inputs.c
PROJECT_SOURCES += static_model.c
PROJECT_SOURCES += static_input.c

include $(LIBIEC_HOME)/make/target_system.mk
include $(LIBIEC_HOME)/make/stack_includes.mk

INCLUDES += -I./inc

all:	$(PROJECT_BINARY_NAME)

include $(LIBIEC_HOME)/make/common_targets.mk

model:	$(PROJECT_ICD_FILE)
	java -jar $(LIBIEC_HOME)/tools/model_generator/genconfig.jar $(PROJECT_ICD_FILE) -ied IED1_XCBR config.cfg
	java -jar model_input_generator/genconfig_input.jar $(PROJECT_ICD_FILE) -ied IED1_XCBR model.cfg

	java -jar $(LIBIEC_HOME)/tools/model_generator/genmodel.jar $(PROJECT_ICD_FILE) -ied IED2_PTOC
	java -jar model_input_generator/genmodel_input.jar $(PROJECT_ICD_FILE) -ied IED2_PTOC

$(PROJECT_BINARY_NAME):	$(PROJECT_SOURCES) $(LIB_NAME)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PROJECT_BINARY_NAME) $(PROJECT_SOURCES) $(INCLUDES) $(LIB_NAME) $(LDLIBS)

clean:
	rm -f $(PROJECT_BINARY_NAME)


