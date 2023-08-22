# whip up some test cases for your graph solution in c++
# case set 0: everyone has a valid path to the base camera
# case set 1: one guy has to daisy chain through 1 node to reach a valid path
# case set 2: multiple one-long chains
# case set 3: one long daisy-chain
# case set 4: multiple long daisy-chains
# case set 5: one guy simply does not have a valid path to the base
# case set 6: multiple guys lack a valid path to the base

from random import randrange, shuffle
# from math import comb
from scipy.special import comb # for compatibility with older pythons
import os
import csv
from math import ceil, sqrt

# pandas dataframes to make for nice metadata?

minCams = 4
maxCams = 128

mink = 2
maxk = 10

Niter = 100

threshold = 30
maxval    = 100

# %%
# constructor 0: everyone has a valid path
fldr = os.path.join('..','csv')
if not os.path.exists(fldr):
    os.mkdir(fldr)

csvfile = os.path.join('..','csv','testCase0.csv')

# cleanup
with open(csvfile,"w+") as _:
    pass

for _ in range(Niter):
    nCams   = randrange(minCams,maxCams+1)
    baseCam = randrange(0,nCams)
    
    # we frankly don't care about thresholds here, we'll just make a bunch of values and set the important ones to be above threshold later
    veclen = nCams*(nCams-1) // 2
    vec = [randrange(maxval+1) for _ in range(veclen)]
        
    # now for each guy connected to the basecam:
    for newCam in range(nCams):
        if newCam == baseCam:
            pass
        else:
            i = min(newCam,baseCam)
            j = max(newCam,baseCam)
            
            idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1) # https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.distance.squareform.html
            
            vec[idx] = randrange(threshold,maxval)
    
    with open(csvfile,"a") as f:
        wrobj = csv.writer(f,delimiter='\t')
        vec = ["nCams",nCams,"baseCam",baseCam,"pairCounts"] + vec
        wrobj.writerow(vec)

# %%
# constructor 1: one fella needs to daisy chain
csvfile = os.path.join('..','csv','testCase1.csv')

# cleanup
with open(csvfile,"w+") as _:
    pass

for _ in range(Niter):
    nCams   = randrange(minCams,maxCams+1)
    baseCam = randrange(0,nCams)
    
    veclen = nCams*(nCams-1) // 2
    vec = [randrange(maxval+1) for _ in range(veclen)]
        
    daisyChainer = randrange(nCams-1)
    daisyChainer = daisyChainer+1 if daisyChainer>=baseCam else daisyChainer
    for newCam in range(nCams):
        if newCam == baseCam:
            pass
        else:
            i = min(newCam,baseCam)
            j = max(newCam,baseCam)
            
            idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1)

            if newCam == daisyChainer:
                vec[idx] = randrange(0,threshold)
                
                # also pick a random index to chain through
                chainIdx = randrange(nCams-2)
                chainIdx = chainIdx+1 if chainIdx >= min(daisyChainer,baseCam) else chainIdx
                chainIdx = chainIdx+1 if chainIdx >= max(daisyChainer,baseCam) else chainIdx
                i = min(chainIdx,daisyChainer)
                j = max(chainIdx,daisyChainer)
                idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1)
                vec[idx] = randrange(ceil(sqrt(2)*threshold),maxval+1)
            else:
                vec[idx] = randrange(ceil(sqrt(2)*threshold),maxval+1)
    
    with open(csvfile,"a") as f:
        wrobj = csv.writer(f,delimiter='\t')
        vec = ["nCams",nCams,"baseCam",baseCam,"daisyChainer",daisyChainer,"pairCounts"] + vec
        wrobj.writerow(vec)
        
# %%
# constructor 2: several fellas need to daisy chain (still producing the occasional failed case...)
csvfile = os.path.join('..','csv','testCase2.csv')

# cleanup
with open(csvfile,"w+") as _:
    pass

for _ in range(Niter):
    nCams   = randrange(minCams,maxCams+1)
    baseCam = randrange(0,nCams)
    
    veclen = nCams*(nCams-1) // 2
    vec = [randrange(maxval+1) for _ in range(veclen)]
        
    nDaisyChainers = randrange(2,nCams//2+1)
    daisyChainers = [i for i in range(nCams-1)] 
    shuffle(daisyChainers)
    daisyChainers = daisyChainers[:nDaisyChainers]
    daisyChainers  = [daisyChainer+1 if daisyChainer>=baseCam else daisyChainer for daisyChainer in daisyChainers]

    for newCam in range(nCams):
        if newCam == baseCam:
            pass
        else:
            i = min(newCam,baseCam)
            j = max(newCam,baseCam)
            
            idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1)

            if newCam in daisyChainers:
                vec[idx] = randrange(0,threshold)
                
                # also pick a random non-chaining index to chain through
                chainIdx = randrange(nCams-len(daisyChainers)-1)

                dc = daisyChainers + [baseCam]
                dc.sort()
                for chainer in dc:
                    chainIdx = chainIdx+1 if chainIdx >= chainer else chainIdx
                i = min(chainIdx,newCam)
                j = max(chainIdx,newCam)
                idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1)
                vec[idx] = randrange(ceil(sqrt(2)*threshold),maxval+1)
            else:
                vec[idx] = randrange(ceil(sqrt(2)*threshold),maxval+1) # guarantees that non-daisychainers have a connection to the base cam.
    
    with open(csvfile,"a") as f:
        wrobj = csv.writer(f,delimiter='\t')
        vec = ["nCams",nCams,"baseCam",baseCam,"daisyChainers"] + daisyChainers + ["pairCounts"] + vec
        wrobj.writerow(vec)
        
# %%
# constructor 3: the existence of a single multi-chain
csvfile = os.path.join('..','csv','testCase3.csv')

# cleanup
with open(csvfile,"w+") as _:
    pass

for iter_ in range(Niter):
    nCams   = randrange(minCams,maxCams+1)
    baseCam = randrange(0,nCams)
    
    veclen = nCams*(nCams-1) // 2
    vec = [randrange(maxval+1) for _ in range(veclen)]
        
    nDaisyChainers = randrange(3,nCams//2+2)
    daisyChainers = [i for i in range(nCams-1)] 
    shuffle(daisyChainers)
    daisyChainers = daisyChainers[:nDaisyChainers]
    daisyChainers  = [daisyChainer+1 if daisyChainer>=baseCam else daisyChainer for daisyChainer in daisyChainers]
    temp           = daisyChainers[:]
    
    for newCam in range(nCams):
        # don't exclude baseCam from newCam this time...
        for chainer in daisyChainers[1:]: # sever all connections with the daisy chainers (except perhaps the last one)
            # note: this is inefficient, I'm running through a bunch of pairs twice.

            # first of all, only do this if the chainer is not your newCam
            if newCam != chainer:
                i = min(newCam,chainer)
                j = max(newCam,chainer)
                
                idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1)
                
                vec[idx] = randrange(0,threshold)
        
        # also make sure to guarantee this newcam connects to the base (if it isn't already the base, and isn't a member of the chainers that have no business connecting to the base)
        if newCam != baseCam and newCam not in daisyChainers[1:]:
            i = min(newCam,baseCam)
            j = max(newCam,baseCam)
            idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1) 

            vec[idx] = randrange(threshold,maxval+1)
            
    # now run thru the daisychain!
    while len(daisyChainers) > 0:
        chainer = daisyChainers.pop()
        if len(daisyChainers)==0: # inefficient...
            i = min(baseCam,chainer) # the last guy in the chain really ought to connect to the base...
            j = max(baseCam,chainer)
        else:
            i = min(daisyChainers[-1],chainer)
            j = max(daisyChainers[-1],chainer)
        
        idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1)
        vec[idx] = randrange(ceil( threshold*sqrt(nDaisyChainers) ),ceil( maxval*sqrt(nDaisyChainers) )+1) # make sure the entries among the daisy chainers actually connect...
    
    with open(csvfile,"a") as f:
        wrobj = csv.writer(f,delimiter='\t')
        vec = ["nCams",nCams,"baseCam",baseCam,"longDaisyChainers"] + temp + ["pairCounts"] + vec
        wrobj.writerow(vec)

# %%
# constructor 4: the existence of several multi-chains
csvfile = os.path.join('..','csv','testCase4.csv')

# cleanup
with open(csvfile,"w+") as _:
    pass

for iter_ in range(Niter):
    nCams   = randrange(minCams,maxCams+1)
    nChains = randrange(2,nCams//2+1)
    baseCam = randrange(0,nCams)
    
    veclen = nCams*(nCams-1) // 2
    vec = [randrange(maxval+1) for _ in range(veclen)]
        
    daisyChainers = []
    for _ in range(nChains):
        nDaisyChainers = randrange(3,nCams//2+2)
        dc = [i for i in range(nCams-1)]
        shuffle(dc)
        dc = dc[:nDaisyChainers]
        dc  = [dc_+1 if dc_>=baseCam else dc_ for dc_ in dc]
        daisyChainers += [dc]
    
    allDaisyChainers = []
    for dc in daisyChainers:
        allDaisyChainers += dc[1:] # exclude the final nodes

    temp = []
    for chain in daisyChainers:
        temp += [chain[:]]

    for newCam in range(nCams):
        # don't exclude baseCam from newCam this time...
        for chainer in allDaisyChainers:
            
            # first of all, only do this if the chainer is not your newCam
            if newCam != chainer:
                i = min(newCam,chainer)
                j = max(newCam,chainer)
                
                idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1)
                
                vec[idx] = randrange(0,threshold)
        
        # also make sure to guarantee this newcam connects to the base (if it isn't already the base, and isn't a member of the chainers which have no business connecting to the base, except the last one but we reset its value later anyway)
        if newCam != baseCam and newCam not in allDaisyChainers:
            i = min(newCam,baseCam)
            j = max(newCam,baseCam)
            idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1)

            vec[idx] = randrange(threshold,maxval+1)
            
    # now run thru the daisychains!
    for dc in daisyChainers:
        while len(dc) > 0:
            chainer = dc.pop()
            if len(dc)==0: # inefficient...
                i = min(baseCam,chainer) # the last guy in the chain really ought to connect to the base...
                j = max(baseCam,chainer)
            else:
                i = min(dc[-1],chainer)
                j = max(dc[-1],chainer)
            
            idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1) # https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.distance.squareform.html
            vec[idx] = randrange(ceil( threshold*sqrt(len(allDaisyChainers)+nChains) ),ceil( maxval*sqrt(len(allDaisyChainers)+nChains) )+1) # make sure the entries among the daisy chainers actually connect...
    
    with open(csvfile,"a") as f:
        wrobj = csv.writer(f,delimiter='\t')
        row = ["nCams",nCams,"baseCam",baseCam]
        for chain in temp:
            row += ["longDaisyChainers"]+chain

        row += ["pairCounts"]+vec
        wrobj.writerow(row)

    # nothing here guarantees uniqueness or non-overlappingness of the long chains. this shouldn't be a problem.


# %%
# constructor 5: the existence of one fella on an island
csvfile = os.path.join('..','csv','testCase5.csv')

# cleanup
with open(csvfile,"w+") as _:
    pass

for _ in range(Niter):
    nCams   = randrange(minCams,maxCams+1)
    baseCam = randrange(0,nCams)
    
    # we frankly don't care about thresholds here, we'll just make a bunch of values and set the important ones to be above threshold later
    veclen = nCams*(nCams-1) // 2
    vec = [randrange(maxval+1) for _ in range(veclen)]

    islander = randrange(nCams-1)
    islander = islander+1 if islander>=baseCam else islander
        
    # now for each guy connected to the basecam:
    for newCam in range(nCams):
        if newCam == baseCam:
            pass
        else:
            i = min(newCam,baseCam)
            j = max(newCam,baseCam)
            
            idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1)

            if newCam != islander:
                vec[idx] = randrange(threshold,maxval)

                # now cut off the islander
                i = min(newCam,islander)
                j = max(newCam,islander)
                
                idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1)
                vec[idx] = randrange(0,ceil(threshold*sqrt(2))-threshold) # make sure it's an ESPECIALLY bad connection, otherwise the tendency will frequently (and correctly) be to try and daisychain the islander, which makes validation difficult! (but which is a behavior that probably needs to be validated per se, huh? ughhh but the way I implemented it is proving to make a quick & dirty unit test much less quick and much more dirty than I anticipated... welcome to edge case town)
            else:
                # make sure the base cam is the best candidate for the islander to connect to (otherwise the unit test programming gets pretty tricky...)
                vec[idx] = randrange( ceil(threshold*sqrt(2))-threshold ,threshold);
    
    with open(csvfile,"a") as f:
        wrobj = csv.writer(f,delimiter='\t')
        vec = ["nCams",nCams,"baseCam",baseCam,"islander",islander,"pairCounts"] + vec
        wrobj.writerow(vec)


# %%
# constructor 6: several fellas lack a valid path
csvfile = os.path.join('..','csv','testCase6.csv')

# cleanup
with open(csvfile,"w+") as _:
    pass

for _ in range(Niter):
    nCams   = randrange(minCams,maxCams+1)
    baseCam = randrange(0,nCams)
    
    # we frankly don't care about thresholds here, we'll just make a bunch of values and set the important ones to be above (AND BELOW) threshold later
    veclen = nCams*(nCams-1) // 2
    vec = [randrange(maxval+1) for _ in range(veclen)]
        
    # now for each guy connected to the basecam:
    nIslanders = randrange(2,nCams//2+1)
    islanders = [i for i in range(nCams-1)] 
    shuffle(islanders)
    islanders = islanders[:nIslanders]
    islanders  = [islander+1 if islander>=baseCam else islander for islander in islanders]

    for newCam in range(nCams):
        if newCam == baseCam:
            pass
        else:
            i = min(newCam,baseCam)
            j = max(newCam,baseCam)
            
            idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1)

            # if newCam is not an islander, just connect it to the base and be done
            if newCam not in islanders:
                vec[idx] = randrange(threshold,maxval)
            else:
                # if newCam *is* an islander, you've got your work cut out for you.
                # first, disconnect it from the base (but not TOO hard)
                vec[idx] = randrange(ceil(threshold*sqrt(2))-threshold,threshold)

                # next, disconnect it HARD from all NON-BASE nodes (inefficiently I might add, but this is a one-time script, not a bottleneck)
                for newerCam in range(nCams):
                    if newerCam == newCam or newerCam == baseCam:
                        pass
                    else:
                        i = min(newCam,newerCam)
                        j = max(newCam,newerCam)
                        idx = comb(nCams,2,exact=True) - comb(nCams-i,2,exact=True) + (j-i-1)

                        vec[idx] = randrange(0,ceil(threshold*sqrt(2))-threshold)




    with open(csvfile,"a") as f:
        wrobj = csv.writer(f,delimiter='\t')
        vec = ["nCams",nCams,"baseCam",baseCam,"islanders"] + islanders + ["pairCounts"] + vec
        wrobj.writerow(vec)