# Adopted from open3d for Jetscan
# Open3D: www.open3d.org
# The MIT License (MIT)
# See license file or visit www.open3d.org for details

import json
import argparse
import time, datetime
import sys
from Utility.file import check_folder_structure
from reconstruction_system.initialize_config import initialize_config

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="regref")
    parser.add_argument("config", help="path to the config file")
    parser.add_argument(
        "--register",
        help="Step 1) register all fragments to detect loop closure",
        action="store_true")

    parser.add_argument("--refine",
                        help="Step 2) refine rough registrations",
                        action="store_true")
    
    parser.add_argument("--debug_mode",
                        help="turn on debug mode",
                        action="store_true")
    args = parser.parse_args()

    if not args.register and not args.refine:
        parser.print_help(sys.stderr)
        sys.exit(1)

    if args.config is not None:
        with open(args.config) as json_file:
            config = json.load(json_file)
            initialize_config(config)
            check_folder_structure(config["path_dataset"])
    assert config is not None

    if args.debug_mode:
        config['debug_mode'] = True
    else:
        config['debug_mode'] = False

    print("====================================")
    print("Configuration")
    print("====================================")
    for key, val in config.items():
        print("%40s : %s" % (key, str(val)))

    times = [0, 0]

    if args.register:
        start_time = time.time()
        from reconstruction_system import register_fragments
        register_fragments.run(config)
        times[0] = time.time() - start_time
    if args.refine:
        start_time = time.time()
        from reconstruction_system import refine_registration
        refine_registration.run(config)
        times[1] = time.time() - start_time

    print("====================================")
    print("Elapsed time (in h:m:s)")
    print("====================================")
    print("- Register fragments  %s" % datetime.timedelta(seconds=times[0]))
    print("- Refine registration %s" % datetime.timedelta(seconds=times[1]))
    print("- Total               %s" % datetime.timedelta(seconds=sum(times)))
    sys.stdout.flush()
