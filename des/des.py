import funs
hextext = raw_input("Enter hex message that is 16 in length: ")
key = "aBCDEFEDCBABCDEF"
key = funs.conv_to_bin(key)
firstperm = funs.op_perm_1(key)
round_keys =funs.round_keys(firstperm)

enc=funs.conv_to_hex(funs.encrypt(hextext, round_keys))
print("Encrypted: "+str(enc))
